#pragma once
#include "VkBuffers.h"

#include <vulkan/vulkan.h>
#include <Eklipse/Renderer/Buffers.h>

#include <vk_mem_alloc.h>

namespace Eklipse
{
	namespace Vulkan
	{
		void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
			VkBuffer& buffer, VmaAllocation& allocation, VmaAllocationCreateFlags vmaFlags);
		void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

		class Buffer : public Eklipse::Buffer
		{
		public:
			virtual void Dispose() override;

			VkBuffer m_buffer{};
			VmaAllocation m_allocation{};
		};

		class VertexBuffer : public Buffer
		{
		public:
			void Setup(const void* data, uint64_t size) override;
		};

		class IndexBuffer : public Buffer
		{
		public:
			void Setup(const void* data, uint64_t size) override;
		};

		class UniformBuffer : public Buffer
		{
		public:
			void Setup(size_t size);
			void Dispose() override;
			void UpdateData(const void* data, size_t size);
		};

		class StagingBuffer : public Buffer
		{
		public:
			void Setup(const void* data, uint64_t size);
			void* m_data;
		};	

		class StorageBuffer : public Buffer
		{
		public:
			void Setup(StagingBuffer& stagingBuffer, VkDeviceSize bufferSize);
		};
	}
}