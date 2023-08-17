#include <precompiled.h>
#include "Vk.h"
#include "VkModel.h"

#include <vk_mem_alloc.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

namespace Eklipse
{
	namespace Vulkan
	{
		ModelAdapter::ModelAdapter(Model& model) : m_model(model) 
		{
			m_uniformBuffer.Setup(sizeof(model.m_ubo));

			m_vertexBuffer.Setup(
				model.m_vertices.data(),
				sizeof(model.m_vertices[0]) * model.m_vertices.size()
			);

			m_indexBuffer.Setup(
				model.m_indices.data(),
				sizeof(model.m_indices[0]) * model.m_indices.size()
			);

			m_texture.Load(model.m_textureData);

			AllocateDescriptorSet();
		}
		void ModelAdapter::Dispose()
		{
			m_uniformBuffer.Dispose();
			m_vertexBuffer.Dispose();
			m_indexBuffer.Dispose();
			m_texture.Dispose();
		}
		void ModelAdapter::Bind(VkCommandBuffer commandBuffer)
		{
			m_uniformBuffer.UpdateData(&m_model.m_ubo, sizeof(m_model.m_ubo));

			VkBuffer vertexBuffer = m_vertexBuffer.m_buffer;
			VkDeviceSize offsets = { 0 };
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer, &offsets);

			VkBuffer indexBuffer = m_indexBuffer.m_buffer;
			vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);

			VkDescriptorSet descriptorSet = m_descriptorSet;
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, g_graphicsPipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
		}
		void ModelAdapter::Draw(VkCommandBuffer commandBuffer)
		{
			uint32_t indexBufferSize = m_model.m_indices.size();
			vkCmdDrawIndexed(commandBuffer, indexBufferSize, 1, 0, 0, 0);
		}
		void ModelAdapter::AllocateDescriptorSet()
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
			bufferInfo.buffer = m_uniformBuffer.m_buffer;
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(m_model.m_ubo);

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

		void ModelManager::Setup(Scene* scene)
		{
			for (auto& model : scene->m_geometry)
			{
				ModelAdapter adapter{ model };
				m_models.push_back(adapter);
			}
		}
		void ModelManager::Dispose()
		{
			for (auto& adapter : m_models)
			{
				adapter.Dispose();
			}
		}
	}
}