#pragma once
#include <vulkan/vulkan.h>

namespace Eklipse
{
	namespace Vulkan
	{
		VkCommandPool CreateCommandPool(int queueFamilyIndex, VkCommandPoolCreateFlagBits flags);
		void CreateCommandBuffers(Vec<VkCommandBuffer>& buffers, int numBuffers, VkCommandPool pool);

		void FreeCommandBuffers(Vec<VkCommandBuffer>& buffers, VkCommandPool pool);

		void BeginRenderPass(VkRenderPass renderPass, VkCommandBuffer commandBuffer, VkFramebuffer frameBuffer, VkExtent2D extent);
		void EndRenderPass(VkCommandBuffer commandBuffer);
		void RecordComputeCommandBuffer();

		VkCommandBuffer BeginSingleCommands();
		void EndSingleCommands(VkCommandBuffer commandBuffer);
	}
}