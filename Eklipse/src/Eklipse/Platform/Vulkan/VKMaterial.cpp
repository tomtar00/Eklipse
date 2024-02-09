#include "precompiled.h"
#include "VKMaterial.h"
#include "VK.h"

#include <Eklipse/Renderer/Renderer.h>

namespace Eklipse
{
	namespace Vulkan
	{
		VKMaterial::VKMaterial(const Path& path, AssetHandle shaderHandle) : Material(path, shaderHandle)
		{
			m_vkShader = std::static_pointer_cast<VKShader>(m_shader);
			CreateDescriptorSets();
		}

		void VKMaterial::Bind()
		{
			EK_PROFILE();
			Material::Bind();
			vkCmdBindDescriptorSets(g_currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_vkShader->GetPipelineLayout(), 0, 1, &m_descriptorSets[g_currentFrame], 0, nullptr);

			for (auto&& [stage, reflection] : m_shader->GetReflections())
			{
				for (auto& pushConstant : reflection.pushConstants)
				{
					vkCmdPushConstants(
						g_currentCommandBuffer, 
						m_vkShader->GetPipelineLayout(), 
						VKShaderStageFromInternalStage(stage), 
						pushConstant.offset, 
						pushConstant.size, 
						m_pushConstants.at(pushConstant.name).pushConstantData.get()
					);
				}
			}
		}
		void VKMaterial::Dispose()
		{
			vkFreeDescriptorSets(g_logicalDevice, g_descriptorPool, static_cast<uint32_t>(m_descriptorSets.size()), m_descriptorSets.data());
		}
		void VKMaterial::ApplyChanges()
		{
			EK_PROFILE();
			Material::ApplyChanges();
			vkDeviceWaitIdle(g_logicalDevice);
			Dispose();
			CreateDescriptorSets();
		}
		
		void VKMaterial::CreateDescriptorSets()
		{
			EK_PROFILE();
			EK_CORE_TRACE("Creating descriptor sets for material {0}", m_name);
			//if (!requiresDescriptorSets) return;

			Vec<VkDescriptorSetLayout> layouts(g_maxFramesInFlight, m_vkShader->GetDescriptorSetLayout());
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
				Vec<VkWriteDescriptorSet> descriptorWrites{};
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
						EK_CORE_TRACE("Binding uniform buffer '{0}' to descriptor set {1} at binding {2} for material {3}", ubo.name, i, ubo.binding, m_name);
					}
				}

				for (auto&& [samplerName, sampler] : m_samplers)
				{
					if (sampler.texture == nullptr) continue;
					auto& texture = std::static_pointer_cast<VKTexture2D>(sampler.texture);

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
					EK_CORE_TRACE("Binding sampler '{0}' to descriptor set {1} at binding {2} for material {3} ({4})", samplerName, i, sampler.binding, m_name, sampler.textureHandle);
				}

				vkUpdateDescriptorSets(g_logicalDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

				for (auto& write : descriptorWrites) 
				{
					delete write.pBufferInfo;
					delete write.pImageInfo;
				}
			}
			EK_CORE_DBG("Created descriptor sets for material {0}", m_name);
		}
	}
}
