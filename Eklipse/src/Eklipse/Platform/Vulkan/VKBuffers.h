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
		void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

		class VKBuffer
		{
		public:
			VkBuffer m_buffer{};
			VmaAllocation m_allocation{};
		};

		class VKVertexBuffer : public Eklipse::VertexBuffer, public VKBuffer
		{
		public:
			VKVertexBuffer(const std::vector<float>& vertices);
			virtual void Bind() const override;
			virtual void Unbind() const override;
			virtual void Dispose() const override;
		};

		class VKIndexBuffer : public Eklipse::IndexBuffer, public VKBuffer
		{
		public:
			VKIndexBuffer(const std::vector<uint32_t>& indices);
			virtual void Bind() const override;
			virtual void Unbind() const override;
			virtual void Dispose() const override;
			virtual size_t GetCount() const override;
		};

		class VKUniformBuffer : public Eklipse::UniformBuffer, public VKBuffer
		{
		public:
			VKUniformBuffer(uint32_t size, uint32_t binding);

			virtual void Dispose() const override;
			virtual void SetData(const void* data, uint32_t size, uint32_t offset) override;
			virtual void* GetBuffer() const override;
		};

		class VKStagingBuffer : public VKBuffer
		{
		public:
			void Setup(const void* data, uint64_t size);
			void Dispose();
			void* m_data;
		};	

		class VKStorageBuffer : public VKBuffer
		{
		public:
			void Setup(VKStagingBuffer& stagingBuffer, VkDeviceSize bufferSize);
			void Dispose();
		};
	}
}