#pragma once
#include "VulkanVertex.h"
#include "VulkanBuffers.h"
#include <vulkan/vulkan.h>

namespace Eklipse
{
	class VulkanBuffer
	{
	public:
		VkBuffer& Buffer();
		VkDeviceMemory& Memory();

	protected:
		static void Create(
			VkDeviceSize size, 
			VkBufferUsageFlags usage, 
			VkMemoryPropertyFlags properties, 
			VkBuffer& buffer, 
			VkDeviceMemory& bufferMemory
		);
		static void Copy(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		static uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

		VkBuffer m_buffer;
		VkDeviceMemory m_bufferMemory;
	};

	class VulkanVertexBuffer : public VulkanBuffer
	{
	public:
		void Init();
		void Shutdown();	

		static const std::vector<VulkanVertex> vertices;

	private:
		void CreateVertexBuffer();
	};

	class VulkanIndexBuffer : public VulkanBuffer
	{
	public:
		void Init();
		void Shutdown();

		static const std::vector<uint16_t> indices;

	private:
		void CreateIndexBuffer();
	};

	class VulkanUniformBuffer : public VulkanBuffer
	{
	public:
		void Init(VkDeviceSize bufferSize);
		void Shutdown();

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