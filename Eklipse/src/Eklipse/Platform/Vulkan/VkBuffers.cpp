#include "precompiled.h"
#include "VkBuffers.h"
#include "VulkanAPI.h"
#include "VkUtils.h"
#include "VkCommands.h"

#include "Vk.h"

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
		// BUFFER ///////////////////////////////////////
		/////////////////////////////////////////////////

		void Buffer::Dispose()
		{
			vmaDestroyBuffer(g_allocator, m_buffer, m_allocation);
		}

		/////////////////////////////////////////////////
		// VERTEX BUFFER ////////////////////////////////
		/////////////////////////////////////////////////

		void VertexBuffer::Setup(const void* data, uint64_t size)
		{
			CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_buffer, m_allocation);

			StagingBuffer stagingBuffer;
			stagingBuffer.Setup(data, size);

			CopyBuffer(stagingBuffer.m_buffer, m_buffer, size);

			stagingBuffer.Dispose();
		}

		/////////////////////////////////////////////////
		// INDEX BUFFER /////////////////////////////////
		/////////////////////////////////////////////////

		void IndexBuffer::Setup(const void* data, uint64_t size)
		{
			CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_buffer, m_allocation);

			StagingBuffer stagingBuffer;
			stagingBuffer.Setup(data, size);

			CopyBuffer(stagingBuffer.m_buffer, m_buffer, size);

			stagingBuffer.Dispose();
		}

		/////////////////////////////////////////////////
		// UNIFORM BUFFER ///////////////////////////////
		/////////////////////////////////////////////////

		void UniformBuffer::Setup(size_t size)
		{
			CreateBuffer(size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				m_buffer, m_allocation, VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT
			);

			VmaAllocationInfo allocInfo;
			vmaGetAllocationInfo(g_allocator, m_allocation, &allocInfo);

			VkResult res = vmaMapMemory(g_allocator, m_allocation, &allocInfo.pMappedData);
			HANDLE_VK_RESULT(res, "MAP UNIFORM DATA");
		}
		void UniformBuffer::Dispose()
		{
			vmaUnmapMemory(g_allocator, m_allocation);
			Buffer::Dispose();
		}
		void UniformBuffer::UpdateData(const void* data, size_t size)
		{
			VmaAllocationInfo allocInfo;
			vmaGetAllocationInfo(g_allocator, m_allocation, &allocInfo);
			memcpy(allocInfo.pMappedData, data, size);
		}

		/////////////////////////////////////////////////
		// STAGING BUFFER ///////////////////////////////
		/////////////////////////////////////////////////

		void StagingBuffer::Setup(const void* data, uint64_t size)
		{
			CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				m_buffer, m_allocation, VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
			);

			vmaMapMemory(g_allocator, m_allocation, &m_data);
			memcpy(m_data, data, (size_t)size);
			vmaUnmapMemory(g_allocator, m_allocation);
		}

		/////////////////////////////////////////////////
		// SHADER STORAGE BUFFER ////////////////////////
		/////////////////////////////////////////////////

		void StorageBuffer::Setup(StagingBuffer& stagingBuffer, VkDeviceSize bufferSize)
		{
			CreateBuffer(bufferSize,
				VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_buffer, m_allocation);

			CopyBuffer(stagingBuffer.m_buffer, m_buffer, bufferSize);
		}
	}
}