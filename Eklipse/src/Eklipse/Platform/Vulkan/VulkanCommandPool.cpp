#include "precompiled.h"
#include "VulkanCommandPool.h"
#include "VulkanAPI.h"

namespace Eklipse
{
    void VulkanCommandPool::Init()
    {
		CreateTransferCommandPool();
	    CreateDrawCommandPool();
    }
	void VulkanCommandPool::InitDrawBuffers(uint32_t numBuffers)
	{
		CreateDrawCommandBuffers(numBuffers);
	}
    void VulkanCommandPool::Shutdown()
    {
		VkDevice device = VulkanAPI::Get().Devices().Device();

		// draw
		for (int i = 0; i < m_drawCommandBuffers.size(); i++)
		{
			vkFreeCommandBuffers(device, m_drawCommandPool, 1, &m_drawCommandBuffers[i]);
		}
		vkDestroyCommandPool(device, m_drawCommandPool, nullptr);

		// transfer
		vkDestroyCommandPool(device, m_transferCommandPool, nullptr);
    }

	//////////////////////////////////////////
	// DRAW //////////////////////////////////
	//////////////////////////////////////////

    void VulkanCommandPool::RecordDrawCommandBuffer(uint32_t bufferIndex, uint32_t imageIndex)
    {
		vkResetCommandBuffer(m_drawCommandBuffers[bufferIndex], 0);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(m_drawCommandBuffers[bufferIndex], &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to begin recording command buffers!");
		}

		VkExtent2D& extent = VulkanAPI::Get().SwapChain().Extent();

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = VulkanAPI::Get().Pipeline().RenderPass();
		renderPassInfo.framebuffer = VulkanAPI::Get().SwapChain().Framebuffers()[imageIndex];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = extent;

		VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(m_drawCommandBuffers[bufferIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(m_drawCommandBuffers[bufferIndex], VK_PIPELINE_BIND_POINT_GRAPHICS,
			VulkanAPI::Get().Pipeline().GraphicsPipeline());

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)extent.width;
		viewport.height = (float)extent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(m_drawCommandBuffers[bufferIndex], 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = extent;
		vkCmdSetScissor(m_drawCommandBuffers[bufferIndex], 0, 1, &scissor);

		VkBuffer vertexBuffers[] = { VulkanAPI::Get().VertexBuffer().Buffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(m_drawCommandBuffers[bufferIndex], 0, 1, vertexBuffers, offsets);

		VkBuffer indexBuffer = VulkanAPI::Get().IndexBuffer().Buffer();
		vkCmdBindIndexBuffer(m_drawCommandBuffers[bufferIndex], indexBuffer, 0, VK_INDEX_TYPE_UINT16);
		
		VkPipelineLayout pipelineLayout = VulkanAPI::Get().Pipeline().Layout();
		VkDescriptorSet& descriptorSet = VulkanAPI::Get().DescriptorPool().DescriptorSets()[bufferIndex];
		vkCmdBindDescriptorSets(m_drawCommandBuffers[bufferIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
		vkCmdDrawIndexed(m_drawCommandBuffers[bufferIndex], static_cast<uint32_t>(VulkanIndexBuffer::indices.size()), 1, 0, 0, 0);
		
		vkCmdEndRenderPass(m_drawCommandBuffers[bufferIndex]);

		if (vkEndCommandBuffer(m_drawCommandBuffers[bufferIndex]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to record command buffer!");
		}
    }

    VkCommandPool& VulkanCommandPool::DrawPool()
    {
        return m_drawCommandPool;
    }

    VkCommandBuffer* VulkanCommandPool::DrawBuffers()
    {
        return m_drawCommandBuffers.data();
    }

    void VulkanCommandPool::CreateDrawCommandPool()
    {
        QueueFamilyIndices queueFamilyIndices = VulkanAPI::Get().FindQueueFamilies(
            VulkanAPI::Get().Devices().PhysicalDevice()
        );

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;

        if (vkCreateCommandPool(VulkanAPI::Get().Devices().Device(), 
            &poolInfo, nullptr, &m_drawCommandPool) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create command pool!");
        }
    }
	void VulkanCommandPool::CreateDrawCommandBuffers(uint32_t numBuffers)
	{
		m_drawCommandBuffers.resize(numBuffers);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = m_drawCommandPool;
		allocInfo.commandBufferCount = numBuffers;

		if (vkAllocateCommandBuffers(VulkanAPI::Get().Devices().Device(),
			&allocInfo, m_drawCommandBuffers.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate command buffers!");
		}
	}

	//////////////////////////////////////////
	// TRANSFER //////////////////////////////
	//////////////////////////////////////////

	VkCommandPool& VulkanCommandPool::TransferPool() { return m_transferCommandPool; }

	void VulkanCommandPool::CreateTransferCommandPool()
	{
		QueueFamilyIndices queueFamilyIndices = VulkanAPI::Get().FindQueueFamilies(
			VulkanAPI::Get().Devices().PhysicalDevice()
		);

		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;

		if (vkCreateCommandPool(VulkanAPI::Get().Devices().Device(),
			&poolInfo, nullptr, &m_transferCommandPool) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create command pool!");
		}
	}

	VkCommandBuffer VulkanCommandPool::BeginSingleCommands()
	{
		VkDevice device = VulkanAPI::Get().Devices().Device();
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = VulkanAPI::Get().CommandPool().TransferPool();
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);
		return commandBuffer;
	}
	void VulkanCommandPool::EndSingleCommands(VkCommandBuffer commandBuffer)
	{
		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		VkQueue graphicsQueue = VulkanAPI::Get().GraphicsQueue();
		vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(graphicsQueue);
	}
}
