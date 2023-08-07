#pragma once
#include <vulkan/vulkan.h>

namespace Eklipse
{
	namespace Vulkan
	{
		void SetupCommandPool();
		void SetupCommandBuffers();
		void DisposeCommandPool();

		void BeginRenderPass(uint32_t bufferIndex, uint32_t imageIndex);
		void EndRenderPass(uint32_t bufferIndex, uint32_t imageIndex);
		void RecordComputeCommandBuffer(uint32_t bufferIndex);

		VkCommandBuffer BeginSingleCommands();
		void EndSingleCommands(VkCommandBuffer commandBuffer);
	}
}