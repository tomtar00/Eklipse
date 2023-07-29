#pragma once
#include "VulkanVertex.h"
#include "VulkanBuffers.h"
#include <vulkan/vulkan.h>

namespace Eklipse
{
	class VulkanBuffer
	{
	public:
		void Shutdown();

		VkBuffer& Buffer();
		VkDeviceMemory& Memory();

		static uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	protected:
		static void Create(
			VkDeviceSize size, 
			VkBufferUsageFlags usage, 
			VkMemoryPropertyFlags properties, 
			VkBuffer& buffer, 
			VkDeviceMemory& bufferMemory
		);
		static void Copy(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);		

		VkBuffer m_buffer;
		VkDeviceMemory m_bufferMemory;
	};

	class VulkanVertexBuffer : public VulkanBuffer
	{
	public:
		void Init(const std::vector<VulkanVertex>& vertices);

		std::vector<VulkanVertex> vertices;

	private:
		void CreateVertexBuffer(const std::vector<VulkanVertex>& vertices);
	};

	class VulkanIndexBuffer : public VulkanBuffer
	{
	public:
		void Init(const std::vector<uint32_t>& indices);

		std::vector<uint32_t> indices;

	private:
		void CreateIndexBuffer(const std::vector<uint32_t>& indices);
	};

	class VulkanStagingBuffer : public VulkanBuffer
	{
	public:
		void Init(VkDeviceSize bufferSize, const void* data);
		void* Data();

	private:
		void* m_data;
	};

	class VulkanUniformBuffer : public VulkanBuffer
	{
	public:
		void Init(VkDeviceSize bufferSize);
		void* Data();

	private:
		void* m_data;
	};

	class VulkanUniformBufferPool
	{
	public:
		void Init(uint32_t numBuffers);
		void Shutdown();

		std::vector<VulkanUniformBuffer>& Buffers();
		void Update(uint32_t bufferIndex);

	private:
		void CreateUniformBuffers(uint32_t numBuffers);

	private:
		std::vector<VulkanUniformBuffer> m_uniformBuffers;
	};
}