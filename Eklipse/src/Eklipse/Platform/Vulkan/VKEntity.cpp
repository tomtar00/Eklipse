#include <precompiled.h>
#include "VK.h"
#include "VKEntity.h"

#include <vk_mem_alloc.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

namespace Eklipse
{
	namespace Vulkan
	{
		VkEntityWrapper::VkEntityWrapper(Entity& entity) : m_entity(entity)
		{
			m_vertexBuffer = VertexBuffer::Create(entity.m_mesh.m_vertices);
			m_indexBuffer = IndexBuffer::Create(entity.m_mesh.m_indices);
			m_uniformBuffer = UniformBuffer::Create(sizeof(entity.m_ubo), 0);
			m_texture.Load(entity.m_mesh.m_textureData);

			AllocateDescriptorSet();
		}
		void VkEntityWrapper::Dispose()
		{
			m_vertexBuffer->Dispose();
			m_indexBuffer->Dispose();
			m_uniformBuffer->Dispose();
			m_texture.Dispose();
		}
		void VkEntityWrapper::Bind(VkCommandBuffer commandBuffer)
		{
			m_vertexBuffer->Bind();
			m_indexBuffer->Bind();

			m_uniformBuffer->SetData(&m_entity.m_ubo, sizeof(m_entity.m_ubo));

			VkDescriptorSet descriptorSet = m_descriptorSet;
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, g_graphicsPipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
		}
		void VkEntityWrapper::Draw(VkCommandBuffer commandBuffer)
		{
			uint32_t numIndices = m_indexBuffer->GetCount();
			vkCmdDrawIndexed(commandBuffer, numIndices, 1, 0, 0, 0);
		}
		void VkEntityWrapper::AllocateDescriptorSet()
		{
			VkDescriptorSetAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfo.descriptorPool = g_descriptorPool;
			allocInfo.descriptorSetCount = 1;
			allocInfo.pSetLayouts = &g_graphicsDescriptorSetLayout;

			VkResult res = vkAllocateDescriptorSets(g_logicalDevice, &allocInfo, &m_descriptorSet);
			HANDLE_VK_RESULT(res, "ALLOCATE GRAPHICS DESCRIPTOR SETS");

			VkWriteDescriptorSet descriptorWrites[2] = { };

			// Uniform buffer
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = (VkBuffer)m_uniformBuffer->GetBuffer();
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(m_entity.m_ubo);

			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = m_descriptorSet;
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &bufferInfo;
			descriptorWrites[0].pImageInfo = VK_NULL_HANDLE;

			// Image sampler
			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = m_texture.m_imageView;
			imageInfo.sampler = m_texture.m_sampler;

			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = m_descriptorSet;
			descriptorWrites[1].dstBinding = 1;
			descriptorWrites[1].dstArrayElement = 0;
			descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[1].descriptorCount = 1;
			descriptorWrites[1].pBufferInfo = VK_NULL_HANDLE;
			descriptorWrites[1].pImageInfo = &imageInfo;
			
			vkUpdateDescriptorSets(g_logicalDevice, 2, descriptorWrites, 0, nullptr);
		}

		void VkEntityManager::Setup(Scene* scene)
		{
			for (auto& model : scene->m_entities)
			{
				VkEntityWrapper wrapper{ model };
				m_wrappers.push_back(wrapper);
			}
		}
		void VkEntityManager::Dispose()
		{
			for (auto& wrapper : m_wrappers)
			{
				wrapper.Dispose();
			}
		}
	}
}