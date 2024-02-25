#pragma once
#include "VKBuffers.h"

#include <vulkan/vulkan.h>
#include <Eklipse/Renderer/Buffers.h>

#include <vk_mem_alloc.h>

namespace Eklipse
{
	namespace Vulkan
	{
		void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
			VkBuffer& buffer, VmaAllocation& allocation, VmaAllocationCreateFlags vmaFlags);
		void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, uint32_t offset = 0);

		class VKBuffer
		{
		public:
			VkBuffer m_buffer{};
			VmaAllocation m_allocation{};
		};

		class VKVertexBuffer : public VertexBuffer, public VKBuffer
		{
		public:
			VKVertexBuffer(const Vec<float>& vertices);
			virtual void SetData(const void* data, uint32_t size) override;
			virtual void Bind() const override;
			virtual void Unbind() const override;
			virtual void Dispose() const override;
		};

		class VKIndexBuffer : public IndexBuffer, public VKBuffer
		{
		public:
			VKIndexBuffer(const Vec<uint32_t>& indices);
			virtual void Bind() const override;
			virtual void Unbind() const override;
			virtual void Dispose() const override;
			virtual size_t GetCount() const override;
		};

		class VKUniformBuffer : public UniformBuffer, public VKBuffer
		{
		public:
			VKUniformBuffer(size_t size, uint32_t binding);

			virtual void Dispose() const override;
			virtual void SetData(const void* data, size_t size, uint32_t offset) override;
			virtual void* GetBuffer() const override;
		};

		class VKStagingBuffer : public VKBuffer
		{
		public:
			VKStagingBuffer(const void* data, size_t size);
			~VKStagingBuffer();
			void* m_data;
		};	

		class VKStorageBuffer : public StorageBuffer, public VKBuffer
		{
		public:
			VKStorageBuffer(size_t size, uint32_t binding);

			virtual void Dispose() const override;
			virtual void SetData(const void* data, size_t size, uint32_t offset) override;
			virtual void* GetBuffer() const override;
		};
	}
}