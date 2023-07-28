#include "precompiled.h"
#include "VulkanDescriptor.h"
#include "VulkanAPI.h"

#include <Eklipse/Renderer/UniformBuffer.h>

namespace Eklipse
{
	/////////////////////////////////////////////////
	// DESCRIPTOR SET LAYOUT ////////////////////////
	/////////////////////////////////////////////////

	void VulkanDescriptorSetLayout::Init()
	{
		CreateDescriptorSetUniformLayout();
	}
	void VulkanDescriptorSetLayout::Shutdown()
	{
		VkDevice device = VulkanAPI::Get().Devices().Device();
		vkDestroyDescriptorSetLayout(device, m_descriptorSetUniformLayout, nullptr);
	}
	VkDescriptorSetLayout& VulkanDescriptorSetLayout::UniformLayout()
	{
		return m_descriptorSetUniformLayout;
	}
	void VulkanDescriptorSetLayout::CreateDescriptorSetUniformLayout()
	{
		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		uboLayoutBinding.pImmutableSamplers = nullptr;

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = 1;
		layoutInfo.pBindings = &uboLayoutBinding;

		VkDevice device = VulkanAPI::Get().Devices().Device();
		if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &m_descriptorSetUniformLayout) != VK_SUCCESS) 
		{
			throw std::runtime_error("failed to create descriptor set layout!");
		}
	}

	/////////////////////////////////////////////////
	// DESCRIPTOR POOL //////////////////////////////
	/////////////////////////////////////////////////

	void VulkanDescriptorPool::Init(uint32_t descriptorCount)
	{
		CreateDescriptorUniformPool(descriptorCount);
		CreateDescriptorUniformSets(descriptorCount);
	}

	void VulkanDescriptorPool::Shutdown()
	{
		VkDevice device = VulkanAPI::Get().Devices().Device();
		vkDestroyDescriptorPool(device, m_descriptorUniformPool, nullptr);
	}

	std::vector<VkDescriptorSet>& VulkanDescriptorPool::DescriptorSets()
	{
		return m_descriptorSets;
	}

	void VulkanDescriptorPool::CreateDescriptorUniformPool(uint32_t descriptorCount)
	{
		VkDescriptorPoolSize poolSize{};
		poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSize.descriptorCount = descriptorCount;

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = 1;
		poolInfo.pPoolSizes = &poolSize;
		poolInfo.maxSets = descriptorCount;

		VkDevice device = VulkanAPI::Get().Devices().Device();
		if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &m_descriptorUniformPool) != VK_SUCCESS) 
		{
			throw std::runtime_error("failed to create descriptor pool!");
		}
	}
	void VulkanDescriptorPool::CreateDescriptorUniformSets(uint32_t descriptorCount)
	{
		VkDescriptorSetLayout descriptorSetLayout = VulkanAPI::Get().DescriptorLayout().UniformLayout();
		std::vector<VkDescriptorSetLayout> layouts(descriptorCount, descriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_descriptorUniformPool;
		allocInfo.descriptorSetCount = descriptorCount;
		allocInfo.pSetLayouts = layouts.data();

		VkDevice device = VulkanAPI::Get().Devices().Device();
		m_descriptorSets.resize(descriptorCount);
		if (vkAllocateDescriptorSets(device, &allocInfo, m_descriptorSets.data()) != VK_SUCCESS) 
		{
			throw std::runtime_error("failed to allocate descriptor sets!");
		}

		VulkanUniformBufferPool& uniformPool = VulkanAPI::Get().UniformBufferPool();
		for (size_t i = 0; i < descriptorCount; i++)
		{
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = uniformPool.Buffers()[i].Buffer();
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(UniformBufferObject);

			VkWriteDescriptorSet descriptorWrite{};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = m_descriptorSets[i];
			descriptorWrite.dstBinding = 0;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = &bufferInfo;

			vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
		}
	}
}
