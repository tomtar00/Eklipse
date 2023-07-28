#pragma once
#include <vulkan/vulkan.h>

namespace Eklipse
{
	class VulkanDrawCommandPool
	{
	public:
		void InitPool();
		void InitBuffers(uint32_t);
		void Shutdown();

		void RecordCommandBuffer(uint32_t bufferIndex, uint32_t imageIndex);

		VkCommandPool& Pool();
		VkCommandBuffer* Buffers();

	private:
		void CreateCommandPool();
		void CreateCommandBuffers(uint32_t numBuffers);

	private:
		VkCommandPool m_commandPool{};
		std::vector<VkCommandBuffer> m_commandBuffers{};
	};

	class VulkanTransferCommandPool
	{
	public:
		void Init();
		void Shutdown();

		VkCommandPool& Pool();

	private:
		void CreateCommandPool();

	private:
		VkCommandPool m_commandPool{};
	};
}