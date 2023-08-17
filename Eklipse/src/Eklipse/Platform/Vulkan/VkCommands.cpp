#include "precompiled.h"

#include "Vk.h"
#include "VkCommands.h"
#include "VkUtils.h"

namespace Eklipse
{
	namespace Vulkan
	{
		VkCommandPool					g_commandPool = VK_NULL_HANDLE;
		std::vector<VkCommandBuffer>	g_drawCommandBuffers{};
		std::vector<VkCommandBuffer>	g_computeCommandBuffers{};

		VkCommandPool CreateCommandPool(int queueFamilyIndex, VkCommandPoolCreateFlagBits flags)
		{
			VkCommandPoolCreateInfo poolInfo{};
			poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			poolInfo.flags = flags;
			poolInfo.queueFamilyIndex = queueFamilyIndex;

			VkCommandPool pool;
			VkResult res = vkCreateCommandPool(g_logicalDevice, &poolInfo, nullptr, &pool);
			HANDLE_VK_RESULT(res, "CREATE COMMAND POOL");

			return pool;
		}
		void CreateCommandBuffers(std::vector<VkCommandBuffer>& buffers, int numBuffers, VkCommandPool pool)
		{
			buffers.resize(numBuffers);

			VkCommandBufferAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocInfo.commandPool = pool;
			allocInfo.commandBufferCount = numBuffers;

			VkResult res = vkAllocateCommandBuffers(g_logicalDevice, &allocInfo, buffers.data());
			HANDLE_VK_RESULT(res, "ALLOCATE COMMAND BUFFERS");
		}

		void SetupCommandPool()
		{
            VkCommandPoolCreateInfo poolInfo{};
            poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
            poolInfo.queueFamilyIndex = g_queueFamilyIndices.graphicsAndComputeFamily;

			VkResult res = vkCreateCommandPool(g_logicalDevice, &poolInfo, nullptr, &g_commandPool);
			HANDLE_VK_RESULT(res, "CREATE COMMAND POOL");
		}
		void SetupCommandBuffers()
		{
			VkResult res;

			// Graphics buffers
			{
				g_drawCommandBuffers.resize(g_maxFramesInFlight);

				VkCommandBufferAllocateInfo allocInfo{};
				allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
				allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
				allocInfo.commandPool = g_commandPool;
				allocInfo.commandBufferCount = g_maxFramesInFlight;

				res = vkAllocateCommandBuffers(g_logicalDevice, &allocInfo, g_drawCommandBuffers.data());
				HANDLE_VK_RESULT(res, "ALLOCATE GRAPHICS COMMAND BUFFERS");
			}

			// Compute buffers
			{
				g_computeCommandBuffers.resize(g_maxFramesInFlight);

				VkCommandBufferAllocateInfo allocInfo{};
				allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
				allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
				allocInfo.commandPool = g_commandPool;
				allocInfo.commandBufferCount = g_maxFramesInFlight;

				res = vkAllocateCommandBuffers(g_logicalDevice, &allocInfo, g_computeCommandBuffers.data());
				HANDLE_VK_RESULT(res, "ALLOCATE COMPUTE COMMAND BUFFERS");
			}
		}
		void DisposeCommandPool()
		{
			// draw
			for (int i = 0; i < g_drawCommandBuffers.size(); i++)
			{
				vkFreeCommandBuffers(g_logicalDevice, g_commandPool, 1, &g_drawCommandBuffers[i]);
			}
			// compute
			for (int i = 0; i < g_computeCommandBuffers.size(); i++)
			{
				vkFreeCommandBuffers(g_logicalDevice, g_commandPool, 1, &g_computeCommandBuffers[i]);
			};
			vkDestroyCommandPool(g_logicalDevice, g_commandPool, nullptr);
		}

		void BeginRenderPass(uint32_t imageIndex)
		{
			vkResetCommandBuffer(g_drawCommandBuffers[g_currentFrame], 0);

			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

			VkResult res;
			res = vkBeginCommandBuffer(g_drawCommandBuffers[g_currentFrame], &beginInfo);
			HANDLE_VK_RESULT(res, "BEGIN DRAW COMMAND BUFFER");

			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = g_renderPass;
			renderPassInfo.framebuffer = g_swapChainFramebuffers[imageIndex];
			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = g_swapChainExtent;

			std::array<VkClearValue, 2> clearValues{};
			clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
			clearValues[1].depthStencil = { 1.0f, 0 };

			renderPassInfo.clearValueCount = clearValues.size();
			renderPassInfo.pClearValues = clearValues.data();

			vkCmdBeginRenderPass(g_drawCommandBuffers[g_currentFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		
			vkCmdBindPipeline(g_drawCommandBuffers[g_currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, g_graphicsPipeline);

			VkViewport viewport{};
			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = (float)g_swapChainExtent.width;
			viewport.height = (float)g_swapChainExtent.height;
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
			vkCmdSetViewport(g_drawCommandBuffers[g_currentFrame], 0, 1, &viewport);

			VkRect2D scissor{};
			scissor.offset = { 0, 0 };
			scissor.extent = g_swapChainExtent;
			vkCmdSetScissor(g_drawCommandBuffers[g_currentFrame], 0, 1, &scissor);
		}
		void EndRenderPass(uint32_t imageIndex)
		{
			vkCmdEndRenderPass(g_drawCommandBuffers[g_currentFrame]);

			VkResult res = vkEndCommandBuffer(g_drawCommandBuffers[g_currentFrame]);
			HANDLE_VK_RESULT(res, "END DRAW COMMAND BUFFER");
		}
		void RecordComputeCommandBuffer()
		{
			VkResult res;
			vkResetCommandBuffer(g_computeCommandBuffers[g_currentFrame], 0);

			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

			res = vkBeginCommandBuffer(g_computeCommandBuffers[g_currentFrame], &beginInfo);
			HANDLE_VK_RESULT(res, "BEGIN COMPUTE COMMAND BUFFER");

			// vkCmdBindPipeline(g_computeCommandBuffers[g_currentFrame], g_particlePipeline);
			// vkCmdBindDescriptorSets(g_computeCommandBuffers[g_currentFrame],
			// 	VK_PIPELINE_BIND_POINT_COMPUTE, g_particleLayout, 0, 1,
			// 	&g_particleDescriptorSets[g_currentFrame], 0, nullptr
			// );
			// 
			// vkCmdDispatch(g_computeCommandBuffers[g_currentFrame], particleCount / 256, 1, 1);
			// 
			// res = vkEndCommandBuffer(g_computeCommandBuffers[g_currentFrame]);
			// HANDLE_VK_RESULT(res, "END COMPUTE COMMAND BUFFER");
		}
		
		VkCommandBuffer BeginSingleCommands()
		{
			VkCommandBufferAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocInfo.commandPool = g_commandPool;
			allocInfo.commandBufferCount = 1;

			VkCommandBuffer commandBuffer;
			vkAllocateCommandBuffers(g_logicalDevice, &allocInfo, &commandBuffer);

			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

			vkBeginCommandBuffer(commandBuffer, &beginInfo);
			return commandBuffer;
		}
		void EndSingleCommands(VkCommandBuffer commandBuffer)
		{
			vkEndCommandBuffer(commandBuffer);

			VkSubmitInfo submitInfo{};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &commandBuffer;

			vkQueueSubmit(g_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
			vkQueueWaitIdle(g_graphicsQueue);
		}
	}
}