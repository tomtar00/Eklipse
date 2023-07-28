#include "precompiled.h"
#include "VulkanCommandPool.h"
#include "VulkanAPI.h"

namespace Eklipse
{
	//////////////////////////////////////////
	// DRAW //////////////////////////////////
	//////////////////////////////////////////

    void VulkanDrawCommandPool::InitPool()
    {
	    CreateCommandPool();
    }
	void VulkanDrawCommandPool::InitBuffers(uint32_t numBuffers)
	{
		CreateCommandBuffers(numBuffers);
	}
    void VulkanDrawCommandPool::Shutdown()
    {
		VkDevice device = VulkanAPI::Get().Devices().Device();
		for (int i = 0; i < m_commandBuffers.size(); i++)
		{
			vkFreeCommandBuffers(device, m_commandPool, 1, &m_commandBuffers[i]);
		}
		vkDestroyCommandPool(device, m_commandPool, nullptr);
    }

    void VulkanDrawCommandPool::RecordCommandBuffer(uint32_t bufferIndex, uint32_t imageIndex)
    {
		vkResetCommandBuffer(m_commandBuffers[bufferIndex], 0);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(m_commandBuffers[bufferIndex], &beginInfo) != VK_SUCCESS)
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

		vkCmdBeginRenderPass(m_commandBuffers[bufferIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(m_commandBuffers[bufferIndex], VK_PIPELINE_BIND_POINT_GRAPHICS,
			VulkanAPI::Get().Pipeline().GraphicsPipeline());

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)extent.width;
		viewport.height = (float)extent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(m_commandBuffers[bufferIndex], 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = extent;
		vkCmdSetScissor(m_commandBuffers[bufferIndex], 0, 1, &scissor);

		VkBuffer vertexBuffers[] = { VulkanAPI::Get().VertexBuffer().Buffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(m_commandBuffers[bufferIndex], 0, 1, vertexBuffers, offsets);

		VkBuffer indexBuffer = VulkanAPI::Get().IndexBuffer().Buffer();
		vkCmdBindIndexBuffer(m_commandBuffers[bufferIndex], indexBuffer, 0, VK_INDEX_TYPE_UINT16);
		
		VkPipelineLayout pipelineLayout = VulkanAPI::Get().Pipeline().Layout();
		VkDescriptorSet& descriptorSet = VulkanAPI::Get().DescriptorPool().DescriptorSets()[bufferIndex];
		vkCmdBindDescriptorSets(m_commandBuffers[bufferIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
		vkCmdDrawIndexed(m_commandBuffers[bufferIndex], static_cast<uint32_t>(VulkanIndexBuffer::indices.size()), 1, 0, 0, 0);
		
		vkCmdEndRenderPass(m_commandBuffers[bufferIndex]);

		if (vkEndCommandBuffer(m_commandBuffers[bufferIndex]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to record command buffer!");
		}
    }

    VkCommandPool& VulkanDrawCommandPool::Pool()
    {
        return m_commandPool;
    }

    VkCommandBuffer* VulkanDrawCommandPool::Buffers()
    {
        return m_commandBuffers.data();
    }

    void VulkanDrawCommandPool::CreateCommandPool()
    {
        QueueFamilyIndices queueFamilyIndices = VulkanAPI::Get().FindQueueFamilies(
            VulkanAPI::Get().Devices().PhysicalDevice()
        );

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;

        if (vkCreateCommandPool(VulkanAPI::Get().Devices().Device(), 
            &poolInfo, nullptr, &m_commandPool) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create command pool!");
        }
    }
	void VulkanDrawCommandPool::CreateCommandBuffers(uint32_t numBuffers)
	{
		m_commandBuffers.resize(numBuffers);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = m_commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = numBuffers;

		if (vkAllocateCommandBuffers(VulkanAPI::Get().Devices().Device(),
			&allocInfo, m_commandBuffers.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate command buffers!");
		}
	}

	//////////////////////////////////////////
	// TRANSFER //////////////////////////////
	//////////////////////////////////////////

	void VulkanTransferCommandPool::Init()
	{
		CreateCommandPool();
	}
	void VulkanTransferCommandPool::Shutdown()
	{
		VkDevice device = VulkanAPI::Get().Devices().Device();
		vkDestroyCommandPool(device, m_commandPool, nullptr);
	}
	VkCommandPool& VulkanTransferCommandPool::Pool() { return m_commandPool; }

	void VulkanTransferCommandPool::CreateCommandPool()
	{
		QueueFamilyIndices queueFamilyIndices = VulkanAPI::Get().FindQueueFamilies(
			VulkanAPI::Get().Devices().PhysicalDevice()
		);

		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;

		if (vkCreateCommandPool(VulkanAPI::Get().Devices().Device(),
			&poolInfo, nullptr, &m_commandPool) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create command pool!");
		}
	}
}
