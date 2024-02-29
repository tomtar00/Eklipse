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
			EK_CORE_PROFILE();
			m_vkShader = std::static_pointer_cast<VKShader>(m_shader);
			CreateDescriptorSets();
		}

		VKMaterial::VKMaterial(const Ref<Shader> shader) : Material(shader)
		{
			EK_CORE_PROFILE();
			m_vkShader = std::static_pointer_cast<VKShader>(m_shader);
			CreateDescriptorSets();
		}

		void VKMaterial::Bind()
		{
			EK_CORE_PROFILE();
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
			EK_CORE_PROFILE();
			if (m_descriptorSets.size() > 0)
				vkFreeDescriptorSets(g_logicalDevice, g_descriptorPool, static_cast<uint32_t>(m_descriptorSets.size()), m_descriptorSets.data());
		}
		void VKMaterial::ApplyChanges(const Path& filePath)
		{
			EK_CORE_PROFILE();
			Material::ApplyChanges(filePath);
			vkDeviceWaitIdle(g_logicalDevice);
			Dispose();
			CreateDescriptorSets();
		}

		void VKMaterial::SetShader(AssetHandle shaderHandle)
		{
			EK_CORE_PROFILE();
            Material::SetShader(shaderHandle);
            m_vkShader = std::static_pointer_cast<VKShader>(m_shader);
		}
		void VKMaterial::SetShader(const Ref<Shader> shader)
		{
			EK_CORE_PROFILE();
			Material::SetShader(shader);
			m_vkShader = std::static_pointer_cast<VKShader>(m_shader);
		}

		void VKMaterial::SetSampler(const String& samplerName, const Ref<Texture2D> texture)
		{
			EK_CORE_PROFILE();
			Material::SetSampler(samplerName, texture);
			auto& vkTexture = std::static_pointer_cast<VKTexture2D>(texture);

			for (size_t i = 0; i < g_maxFramesInFlight; i++)
			{
				VkDescriptorImageInfo imageInfo{};
				imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				imageInfo.imageView = vkTexture->GetImageView();
				imageInfo.sampler = vkTexture->GetSampler();

                VkWriteDescriptorSet descriptorWrite{};
                descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrite.dstSet = m_descriptorSets[i];
                descriptorWrite.dstBinding = m_samplers.at(samplerName).binding;
                descriptorWrite.dstArrayElement = 0;
                descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                descriptorWrite.descriptorCount = 1;
                descriptorWrite.pImageInfo = &imageInfo;

                vkUpdateDescriptorSets(g_logicalDevice, 1, &descriptorWrite, 0, nullptr);
            }
		}
		
		void VKMaterial::CreateDescriptorSets()
		{
			EK_CORE_PROFILE();
			EK_CORE_TRACE("Creating descriptor sets for material {0}", Name);
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
						EK_CORE_TRACE("Binding uniform buffer '{0}' to descriptor set {1} at binding {2} for material {3}", ubo.name, i, ubo.binding, Name);
					}

					for (auto& storage : reflection.storageBuffers)
					{
						Ref<VKStorageBuffer> storageBuffer = std::static_pointer_cast<VKStorageBuffer>(
							Renderer::GetStorageBuffer(storage.name)
						);

						VkDescriptorBufferInfo* bufferInfo = new VkDescriptorBufferInfo;
						bufferInfo->buffer = storageBuffer->m_buffer;
						bufferInfo->offset = 0;
						bufferInfo->range = storageBuffer->GetSize();

						VkWriteDescriptorSet descriptorWrite{};
						descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
						descriptorWrite.dstSet = m_descriptorSets[i];
						descriptorWrite.dstBinding = storage.binding;
						descriptorWrite.dstArrayElement = 0;
						descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
						descriptorWrite.descriptorCount = 1;
						descriptorWrite.pBufferInfo = bufferInfo;

						descriptorWrites.push_back(descriptorWrite);
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
					EK_CORE_TRACE("Binding sampler '{0}' to descriptor set {1} at binding {2} for material {3} ({4})", samplerName, i, sampler.binding, Name, sampler.textureHandle);
				}

				vkUpdateDescriptorSets(g_logicalDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

				for (auto& write : descriptorWrites) 
				{
					delete write.pBufferInfo;
					delete write.pImageInfo;
				}
			}
			EK_CORE_DBG("Created descriptor sets for material {0}", Name);
		}
	}
}
