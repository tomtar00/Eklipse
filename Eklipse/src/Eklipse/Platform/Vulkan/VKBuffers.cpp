#include "precompiled.h"
#include "VKBuffers.h"
#include "VulkanAPI.h"
#include "VKUtils.h"
#include "VKCommands.h"

#include "VK.h"

#include <vulkan/vulkan.h>

namespace Eklipse
{
	namespace Vulkan
	{
		void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
			VkBuffer& buffer, VmaAllocation& allocation, VmaAllocationCreateFlags vmaFlags = 0)
		{
			VkBufferCreateInfo bufferInfo{};
			bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferInfo.size = size;
			bufferInfo.usage = usage;
			bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			VmaAllocationCreateInfo vmaAllocInfo = {};
			vmaAllocInfo.usage = VMA_MEMORY_USAGE_AUTO;
			vmaAllocInfo.flags = vmaFlags;

			VkResult res = vmaCreateBuffer(g_allocator, &bufferInfo, &vmaAllocInfo, &buffer, &allocation, nullptr);
			HANDLE_VK_RESULT(res, "CREATE BUFFER");
		}
		void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
		{
			VkCommandBuffer commandBuffer = BeginSingleCommands();

			VkBufferCopy copyRegion{};
			copyRegion.size = size;
			vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

			EndSingleCommands(commandBuffer);
		}

		/////////////////////////////////////////////////
		// VERTEX BUFFER ////////////////////////////////
		/////////////////////////////////////////////////

		VKVertexBuffer::VKVertexBuffer(const std::vector<float>& vertices)
		{
			m_count = vertices.size();
			size_t size = sizeof(float) * vertices.size();
			CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_buffer, m_allocation);

			VKStagingBuffer stagingBuffer(vertices.data(), size);
			CopyBuffer(stagingBuffer.m_buffer, m_buffer, size);
		}
		void VKVertexBuffer::SetData(const void* data, uint32_t size)
		{
			VKStagingBuffer stagingBuffer(data, size);
			CopyBuffer(stagingBuffer.m_buffer, m_buffer, size);
		}
		void VKVertexBuffer::Bind() const
		{
			VkDeviceSize offsets = { 0 };
			vkCmdBindVertexBuffers(g_currentCommandBuffer, 0, 1, &m_buffer, &offsets);
		}
		void VKVertexBuffer::Unbind() const
		{
			vmaUnmapMemory(g_allocator, m_allocation);
		}
		void VKVertexBuffer::Dispose() const
		{
			vmaDestroyBuffer(g_allocator, m_buffer, m_allocation);
		}

		/////////////////////////////////////////////////
		// INDEX BUFFER /////////////////////////////////
		/////////////////////////////////////////////////

		VKIndexBuffer::VKIndexBuffer(const std::vector<uint32_t>& indices)
		{
			m_count = indices.size();
			size_t size = sizeof(indices[0]) * m_count;
			CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_buffer, m_allocation);

			VKStagingBuffer stagingBuffer(indices.data(), size);
			CopyBuffer(stagingBuffer.m_buffer, m_buffer, size);
		}
		void VKIndexBuffer::Bind() const
		{
			vkCmdBindIndexBuffer(g_currentCommandBuffer, m_buffer, 0, VK_INDEX_TYPE_UINT32);
		}
		void VKIndexBuffer::Unbind() const
		{
			vmaUnmapMemory(g_allocator, m_allocation);
		}
		void VKIndexBuffer::Dispose() const
		{
			vmaDestroyBuffer(g_allocator, m_buffer, m_allocation);
		}
		size_t VKIndexBuffer::GetCount() const
		{
			return m_count;
		}

		/////////////////////////////////////////////////
		// UNIFORM BUFFER ///////////////////////////////
		/////////////////////////////////////////////////

		VKUniformBuffer::VKUniformBuffer(uint32_t size, uint32_t binding)
		{
			CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				m_buffer, m_allocation, VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT
			);

			//m_mappedData = nullptr;

			VmaAllocationInfo allocInfo;
			vmaGetAllocationInfo(g_allocator, m_allocation, &allocInfo);
			VkResult res = vmaMapMemory(g_allocator, m_allocation, &allocInfo.pMappedData);
			HANDLE_VK_RESULT(res, "MAP UNIFORM DATA");
			
			//=================

			/*VkBufferCreateInfo bufferInfo = {};
			bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferInfo.size = size;
			bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
			bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			if (vkCreateBuffer(g_logicalDevice, &bufferInfo, nullptr, &m_buffer) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create uniform buffer");
			}

			VkMemoryRequirements memRequirements;
			vkGetBufferMemoryRequirements(g_logicalDevice, m_buffer, &memRequirements);

			VkMemoryAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.allocationSize = memRequirements.size;
			allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

			if (vkAllocateMemory(g_logicalDevice, &allocInfo, nullptr, &m_memory) != VK_SUCCESS) {
				throw std::runtime_error("failed to allocate buffer memory!");
			}

			vkBindBufferMemory(g_logicalDevice, m_buffer, m_memory, 0);*/
		}
		void VKUniformBuffer::Dispose() const
		{
			vmaUnmapMemory(g_allocator, m_allocation);
			vmaDestroyBuffer(g_allocator, m_buffer, m_allocation);

			// vkDestroyBuffer(g_logicalDevice, m_buffer, nullptr);
			// vkFreeMemory(g_logicalDevice, m_memory, nullptr);
		}
		void VKUniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset = 0)
		{
			
			VmaAllocationInfo allocInfo;
			vmaGetAllocationInfo(g_allocator, m_allocation, &allocInfo);
			memcpy(allocInfo.pMappedData, data, size);
			
			// vmaMapMemory(g_allocator, m_allocation, &m_mappedData);
			// memcpy(m_mappedData, data, size);
			// vmaUnmapMemory(g_allocator, m_allocation);

			/*vkMapMemory(g_logicalDevice, m_memory, offset, size, 0, &m_mappedData);
			memcpy(m_mappedData, data, size);
			vkUnmapMemory(g_logicalDevice, m_memory);*/
		}
		void* VKUniformBuffer::GetBuffer() const
		{
			return m_buffer;
		}

		/////////////////////////////////////////////////
		// STAGING BUFFER ///////////////////////////////
		/////////////////////////////////////////////////

		VKStagingBuffer::VKStagingBuffer(const void* data, size_t size)
		{
			CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				m_buffer, m_allocation, VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
			);

			vmaMapMemory(g_allocator, m_allocation, &m_data);
			memcpy(m_data, data, (size_t)size);
			vmaUnmapMemory(g_allocator, m_allocation);
		}
		VKStagingBuffer::~VKStagingBuffer()
		{
			vmaDestroyBuffer(g_allocator, m_buffer, m_allocation);
		}

		/////////////////////////////////////////////////
		// SHADER STORAGE BUFFER ////////////////////////
		/////////////////////////////////////////////////

		void VKStorageBuffer::Setup(VKStagingBuffer& stagingBuffer, VkDeviceSize bufferSize)
		{
			CreateBuffer(bufferSize,
				VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_buffer, m_allocation);

			CopyBuffer(stagingBuffer.m_buffer, m_buffer, bufferSize);
		}
		void VKStorageBuffer::Dispose()
		{
			vmaDestroyBuffer(g_allocator, m_buffer, m_allocation);
		}
	}
}