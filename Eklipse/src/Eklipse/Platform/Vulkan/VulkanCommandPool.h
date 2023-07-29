#pragma once
#include <vulkan/vulkan.h>

namespace Eklipse
{
	class VulkanCommandPool
	{
	public:
		void Init();
		void InitDrawBuffers(uint32_t);
		void Shutdown();

		void RecordDrawCommandBuffer(uint32_t bufferIndex, uint32_t imageIndex);

		VkCommandBuffer BeginSingleCommands();
		void EndSingleCommands(VkCommandBuffer commandBuffer);

		VkCommandPool& DrawPool();
		VkCommandPool& TransferPool();
		VkCommandBuffer* DrawBuffers();

	private:
		void CreateDrawCommandPool();
		void CreateTransferCommandPool();
		void CreateDrawCommandBuffers(uint32_t numBuffers);

	private:
		VkCommandPool m_drawCommandPool{};
		std::vector<VkCommandBuffer> m_drawCommandBuffers{};

		VkCommandPool m_transferCommandPool{};
	};
}