#pragma once
#include <vulkan/vulkan.h>

namespace Eklipse
{
	namespace Vulkan
	{
		VkCommandPool CreateCommandPool(int queueFamilyIndex, VkCommandPoolCreateFlagBits flags);
		void CreateCommandBuffers(std::vector<VkCommandBuffer>& buffers, int numBuffers, VkCommandPool pool);

		void FreeCommandBuffers(std::vector<VkCommandBuffer>& buffers, VkCommandPool pool);

		void BeginRenderPass(VkRenderPass renderPass, VkCommandBuffer commandBuffer, VkFramebuffer frameBuffer, VkExtent2D extent);
		void EndRenderPass(VkCommandBuffer commandBuffer);
		void RecordComputeCommandBuffer();

		VkCommandBuffer BeginSingleCommands();
		void EndSingleCommands(VkCommandBuffer commandBuffer);
	}
}