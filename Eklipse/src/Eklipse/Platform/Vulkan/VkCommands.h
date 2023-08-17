#pragma once
#include <vulkan/vulkan.h>

namespace Eklipse
{
	namespace Vulkan
	{
		VkCommandPool CreateCommandPool(int queueFamilyIndex, VkCommandPoolCreateFlagBits flags);
		void CreateCommandBuffers(std::vector<VkCommandBuffer>& buffers, int numBuffers, VkCommandPool pool);

		void DisposeCommandPool();

		void BeginRenderPass(uint32_t imageIndex);
		void EndRenderPass(uint32_t imageIndex);
		void RecordComputeCommandBuffer();

		VkCommandBuffer BeginSingleCommands();
		void EndSingleCommands(VkCommandBuffer commandBuffer);
	}
}