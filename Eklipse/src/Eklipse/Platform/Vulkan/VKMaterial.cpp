#include "precompiled.h"
#include "VKMaterial.h"
#include "VK.h"

#include <Eklipse/Renderer/Renderer.h>
#include <Eklipse/Scene/Assets.h>

namespace Eklipse
{
	namespace Vulkan
	{
		VKMaterial::VKMaterial(const std::filesystem::path& path) : Material(path)
		{
			m_vkShader = std::static_pointer_cast<VKShader>(m_shader);
			CreateDescriptorSets();
		}
		void VKMaterial::Bind()
		{
			Material::Bind();
			vkCmdBindDescriptorSets(g_currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_vkShader->GetPipelineLayout(), 0, 1, &m_descriptorSets[g_currentFrame], 0, nullptr);

			for (auto&& [stage, reflection] : m_shader->GetReflections())
			{
				for (auto& pushConstant : reflection.pushConstants)
				{
					vkCmdPushConstants(g_currentCommandBuffer, m_vkShader->GetPipelineLayout(), 
						VKShaderStageFromInternalStage(stage), 0, pushConstant.size, m_pushConstants[pushConstant.name].pushConstantData.get());
				}
			}
		}
		void VKMaterial::Dispose()
		{
			Material::Dispose();
			vkFreeDescriptorSets(g_logicalDevice, g_descriptorPool, static_cast<uint32_t>(m_descriptorSets.size()), m_descriptorSets.data());
		}
		void VKMaterial::CreateDescriptorSets()
		{
			//if (!requiresDescriptorSets) return;

			std::vector<VkDescriptorSetLayout> layouts(g_maxFramesInFlight, m_vkShader->GetDescriptorSetLayout());
			VkDescriptorSetAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfo.descriptorPool = g_descriptorPool; // TODO: create some descriptor pool manager
			allocInfo.descriptorSetCount = g_maxFramesInFlight;
			allocInfo.pSetLayouts = layouts.data();

			m_descriptorSets.resize(g_maxFramesInFlight);
			VkResult res = vkAllocateDescriptorSets(g_logicalDevice, &allocInfo, m_descriptorSets.data());
			HANDLE_VK_RESULT(res, "ALLOCATE DESCRIPTOR SETS");		

			for (size_t i = 0; i < g_maxFramesInFlight; i++)
			{
				std::vector<VkWriteDescriptorSet> descriptorWrites{};
				for (auto&& [stage, reflection] : m_shader->GetReflections())
				{
					for (auto& ubo : reflection.uniformBuffers)
					{
						Ref<VKUniformBuffer> uniformBuffer = std::static_pointer_cast<VKUniformBuffer>(
							Renderer::GetUniformBuffer(ubo.name)
						);

						VkDescriptorBufferInfo* bufferInfo = new VkDescriptorBufferInfo;
						bufferInfo->buffer = uniformBuffer->m_buffer;
						bufferInfo->offset = 0;
						bufferInfo->range = ubo.size;

						VkWriteDescriptorSet descriptorWrite{};
						descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
						descriptorWrite.dstSet = m_descriptorSets[i];
						descriptorWrite.dstBinding = ubo.binding;
						descriptorWrite.dstArrayElement = 0;
						descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
						descriptorWrite.descriptorCount = 1;
						descriptorWrite.pBufferInfo = bufferInfo;

						descriptorWrites.push_back(descriptorWrite);
						EK_CORE_INFO("Binding uniform buffer '{0}' to descriptor set {1} at binding {2}", ubo.name, i, ubo.binding);
					}

					uint32_t index = 0;
					for (auto& sampler : reflection.samplers)
					{
						if (m_sampledTextures.size() <= index) break;
						auto& texture = std::static_pointer_cast<VKTexture2D>(m_sampledTextures[index++]);

						VkDescriptorImageInfo* imageInfo = new VkDescriptorImageInfo;
						imageInfo->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
						imageInfo->imageView = texture->GetImageView();
						imageInfo->sampler = texture->GetSampler();

						VkWriteDescriptorSet descriptorWrite{};
						descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
						descriptorWrite.dstSet = m_descriptorSets[i];
						descriptorWrite.dstBinding = sampler.binding;
						descriptorWrite.dstArrayElement = 0;
						descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
						descriptorWrite.descriptorCount = 1;
						descriptorWrite.pImageInfo = imageInfo;

						descriptorWrites.push_back(descriptorWrite);
						EK_CORE_INFO("Binding sampler '{0}' to descriptor set {1} at binding {2}", sampler.name, i, sampler.binding);
					}
				}

				vkUpdateDescriptorSets(g_logicalDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

				for (auto& write : descriptorWrites) 
				{
					delete write.pBufferInfo;
					delete write.pImageInfo;
				}
			}
		}
	}
}
