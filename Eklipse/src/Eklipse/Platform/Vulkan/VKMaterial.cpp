#include "precompiled.h"
#include "VKMaterial.h"
#include "VK.h"

namespace Eklipse
{
	namespace Vulkan
	{
		VKMaterial::VKMaterial(Ref<Shader> shader) : Material(shader)
		{
			m_vkShader = std::static_pointer_cast<VKShader>(shader);
			CreateDescriptorSets();
		}
		VKMaterial::~VKMaterial()
		{
		
		}
		void VKMaterial::Bind()
		{
			m_vkShader->Bind();
			vkCmdBindDescriptorSets(g_currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_vkShader->GetPipelineLayout(), 0, 1, &m_descriptorSets[g_currentFrame], 0, nullptr);
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
						// TODO: check if this uniform buffer already exists
						
						Ref<VKUniformBuffer> uniformBuffer = CreateRef<VKUniformBuffer>(ubo.size, ubo.binding);
						m_uniformBuffers.push_back(uniformBuffer);

						VkDescriptorBufferInfo bufferInfo{};
						bufferInfo.buffer = uniformBuffer->m_buffer;
						bufferInfo.offset = 0;
						bufferInfo.range = ubo.size;

						VkWriteDescriptorSet descriptorWrite{};
						descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
						descriptorWrite.dstSet = m_descriptorSets[i];
						descriptorWrite.dstBinding = ubo.binding;
						descriptorWrite.dstArrayElement = 0;
						descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
						descriptorWrite.descriptorCount = 1;
						descriptorWrite.pBufferInfo = &bufferInfo;

						descriptorWrites.push_back(descriptorWrite);
					}
					for (auto& sampler : reflection.samplers)
					{
						// Creating dummy texture, fix it to support loading from file (.mat) in assets library
						TextureInfo textureInfo = {};
						textureInfo.width = 1;
						textureInfo.height = 1;
						textureInfo.mipMapLevel = 1;
						textureInfo.samples = 1;
						textureInfo.imageFormat = ImageFormat::RGBA8;
						textureInfo.imageAspect = ImageAspect::COLOR;
						textureInfo.imageUsage = ImageUsage::SAMPLED;

						Ref<VKTexture2D> texture = CreateRef<VKTexture2D>(textureInfo);
						m_textures.push_back(texture);

						VkDescriptorImageInfo imageInfo{};
						imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
						imageInfo.imageView = texture->GetImageView();
						imageInfo.sampler = texture->GetSampler();

						VkWriteDescriptorSet descriptorWrite{};
						descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
						descriptorWrite.dstSet = m_descriptorSets[i];
						descriptorWrite.dstBinding = sampler.binding;
						descriptorWrite.dstArrayElement = 0;
						descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
						descriptorWrite.descriptorCount = 1;
						descriptorWrite.pImageInfo = &imageInfo;

						descriptorWrites.push_back(descriptorWrite);
					}
				}

				vkUpdateDescriptorSets(g_logicalDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
			}
		}
	}
}
