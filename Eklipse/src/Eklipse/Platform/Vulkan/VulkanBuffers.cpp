#include "precompiled.h"
#include "VulkanBuffers.h"
#include "VulkanVertex.h"
#include "VulkanAPI.h"

#include <Eklipse/Renderer/UniformBuffer.h>

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Eklipse
{
	/////////////////////////////////////////////////
	// BUFFER ///////////////////////////////////////
	/////////////////////////////////////////////////

	void VulkanBuffer::Shutdown()
	{
		VkDevice device = VulkanAPI::Get().Devices().Device();
		vkDestroyBuffer(device, m_buffer, nullptr);
		vkFreeMemory(device, m_bufferMemory, nullptr);
	}

	VkBuffer& VulkanBuffer::Buffer()
	{
		return m_buffer;
	}
	VkDeviceMemory& VulkanBuffer::Memory()
	{
		return m_bufferMemory;
	}
	void VulkanBuffer::Create(
		VkDeviceSize size,
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags properties,
		VkBuffer& buffer,
		VkDeviceMemory& bufferMemory)
	{
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VkDevice device = VulkanAPI::Get().Devices().Device();
		if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) 
		{
			throw std::runtime_error("failed to create buffer!");
		}

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) 
		{
			throw std::runtime_error("failed to allocate buffer memory!");
		}

		vkBindBufferMemory(device, buffer, bufferMemory, 0);
	}

	void VulkanBuffer::Copy(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
	{
		VkCommandBuffer commandBuffer = VulkanAPI::Get().CommandPool().BeginSingleCommands();

		VkBufferCopy copyRegion{};
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

		VulkanAPI::Get().CommandPool().EndSingleCommands(commandBuffer);
	}

	uint32_t VulkanBuffer::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties memProperties;

		VkPhysicalDevice physicalDevice = VulkanAPI::Get().Devices().PhysicalDevice();
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		{
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				return i;
			}
		}

		throw std::runtime_error("failed to find suitable memory type!");
	}

	/////////////////////////////////////////////////
	// VERTEX BUFFER ////////////////////////////////
	/////////////////////////////////////////////////

	//const std::vector<VulkanVertex> VulkanVertexBuffer::vertices =
	//{
	//	{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
	//	{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
	//	{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
	//	{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

	//	{{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
	//	{{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
	//	{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
	//	{{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
	//};

	void VulkanVertexBuffer::Init(const std::vector<VulkanVertex>& vertices)
	{
		this->vertices = vertices;
		CreateVertexBuffer(vertices);
	}
	void VulkanVertexBuffer::CreateVertexBuffer(const std::vector<VulkanVertex>& vertices)
	{
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

		Create(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_buffer, m_bufferMemory);

		VulkanStagingBuffer stagingBuffer;
		stagingBuffer.Init(bufferSize, vertices.data());

		Copy(stagingBuffer.Buffer(), m_buffer, bufferSize);

		stagingBuffer.Shutdown();
	}

	/////////////////////////////////////////////////
	// INDEX BUFFER /////////////////////////////////
	/////////////////////////////////////////////////

	/*const std::vector<uint16_t> VulkanIndexBuffer::indices =
	{
		0, 1, 2, 2, 3, 0,
		4, 5, 6, 6, 7, 4
	};*/

	void VulkanIndexBuffer::Init(const std::vector<uint32_t>& indices)
	{
		this->indices = indices;
		CreateIndexBuffer(indices);
	}
	void VulkanIndexBuffer::CreateIndexBuffer(const std::vector<uint32_t>& indices)
	{
		VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		Create(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
			stagingBuffer, stagingBufferMemory);

		VkDevice device = VulkanAPI::Get().Devices().Device();
		void* data;
		vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, indices.data(), (size_t)bufferSize);
		vkUnmapMemory(device, stagingBufferMemory);

		Create(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_buffer, m_bufferMemory);

		Copy(stagingBuffer, m_buffer, bufferSize);

		vkDestroyBuffer(device, stagingBuffer, nullptr);
		vkFreeMemory(device, stagingBufferMemory, nullptr);
	}

	/////////////////////////////////////////////////
	// STAGING BUFFER ///////////////////////////////
	/////////////////////////////////////////////////

	void VulkanStagingBuffer::Init(VkDeviceSize bufferSize, const void* data)
	{
		Create(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			m_buffer, m_bufferMemory
		);

		VkDevice device = VulkanAPI::Get().Devices().Device();
		vkMapMemory(device, m_bufferMemory, 0, bufferSize, 0, &m_data);
		memcpy(m_data, data, (size_t)bufferSize);
		vkUnmapMemory(device, m_bufferMemory);
	}
	void* VulkanStagingBuffer::Data()
	{
		return m_data;
	}

	/////////////////////////////////////////////////
	// UNIFORM BUFFER ///////////////////////////////
	/////////////////////////////////////////////////

	void VulkanUniformBufferPool::Init(uint32_t numBuffers)
	{
		CreateUniformBuffers(numBuffers);
	}
	void VulkanUniformBufferPool::Shutdown()
	{
		for (int i = 0; i < m_uniformBuffers.size(); i++)
		{
			m_uniformBuffers[i].Shutdown();
		}
	}
	std::vector<VulkanUniformBuffer>& VulkanUniformBufferPool::Buffers()
	{
		return m_uniformBuffers;
	}
	void VulkanUniformBufferPool::Update(uint32_t bufferIndex)
	{
		static auto startTime = std::chrono::high_resolution_clock::now();

		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		float width = VulkanAPI::Get().SwapChain().Extent().width;
		float height = VulkanAPI::Get().SwapChain().Extent().height;
		float aspectRatio = width / height;

		UniformBufferObject ubo{};
		ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.proj = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 10.0f);
		ubo.proj[1][1] *= -1;

		memcpy(m_uniformBuffers[bufferIndex].Data(), &ubo, sizeof(ubo));
	}
	void VulkanUniformBufferPool::CreateUniformBuffers(uint32_t numBuffers)
	{
		VkDeviceSize bufferSize = sizeof(UniformBufferObject);
		m_uniformBuffers.resize(numBuffers);

		for (size_t i = 0; i < numBuffers; i++) 
		{
			m_uniformBuffers[i].Init(bufferSize);
		}
	}

	void VulkanUniformBuffer::Init(VkDeviceSize bufferSize)
	{
		Create(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			m_buffer, m_bufferMemory
		);

		VkDevice device = VulkanAPI::Get().Devices().Device();
		vkMapMemory(device, m_bufferMemory, 0, bufferSize, 0, &m_data);
	}
	void* VulkanUniformBuffer::Data()
	{
		return m_data;
	}
}