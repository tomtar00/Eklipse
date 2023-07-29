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
		CreateDescriptorSetLayout();
	}
	void VulkanDescriptorSetLayout::Shutdown()
	{
		VkDevice device = VulkanAPI::Get().Devices().Device();
		vkDestroyDescriptorSetLayout(device, m_uniformDescriptorSetLayout, nullptr);
	}
	VkDescriptorSetLayout& VulkanDescriptorSetLayout::UniformLayout()
	{
		return m_uniformDescriptorSetLayout;
	}
	void VulkanDescriptorSetLayout::CreateDescriptorSetLayout()
	{
		// Uniform buffers binding
		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		uboLayoutBinding.pImmutableSamplers = nullptr;

		// Texture sampler binding
		VkDescriptorSetLayoutBinding samplerLayoutBinding{};
		samplerLayoutBinding.binding = 1;
		samplerLayoutBinding.descriptorCount = 1;
		samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		samplerLayoutBinding.pImmutableSamplers = nullptr;

		std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		VkDevice device = VulkanAPI::Get().Devices().Device();
		if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &m_uniformDescriptorSetLayout) != VK_SUCCESS) 
		{
			throw std::runtime_error("failed to create descriptor set layout!");
		}
	}

	/////////////////////////////////////////////////
	// DESCRIPTOR POOL //////////////////////////////
	/////////////////////////////////////////////////

	void VulkanDescriptorPool::Init(uint32_t descriptorCount)
	{
		CreateDescriptorPool(descriptorCount);
		CreateUniformDescriptorSets(descriptorCount);
	}

	void VulkanDescriptorPool::Shutdown()
	{
		VkDevice device = VulkanAPI::Get().Devices().Device();
		vkDestroyDescriptorPool(device, m_descriptorPool, nullptr);
	}

	std::vector<VkDescriptorSet>& VulkanDescriptorPool::DescriptorSets()
	{
		return m_descriptorSets;
	}

	void VulkanDescriptorPool::CreateDescriptorPool(uint32_t descriptorCount)
	{
		// Uniform buffer
		VkDescriptorPoolSize uboPoolSize{};
		uboPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboPoolSize.descriptorCount = descriptorCount;

		// Texture sampler
		VkDescriptorPoolSize samplerPoolSize{};
		samplerPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerPoolSize.descriptorCount = descriptorCount;

		std::array<VkDescriptorPoolSize, 2> poolSizes = { uboPoolSize, samplerPoolSize };

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = descriptorCount;

		VkDevice device = VulkanAPI::Get().Devices().Device();
		if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS) 
		{
			throw std::runtime_error("failed to create descriptor pool!");
		}
	}
	void VulkanDescriptorPool::CreateUniformDescriptorSets(uint32_t descriptorCount)
	{
		VkDescriptorSetLayout descriptorSetLayout = VulkanAPI::Get().DescriptorLayout().UniformLayout();
		std::vector<VkDescriptorSetLayout> layouts(descriptorCount, descriptorSetLayout);

		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_descriptorPool;
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
			// Uniform buffer
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = uniformPool.Buffers()[i].Buffer();
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(UniformBufferObject);

			VkWriteDescriptorSet uboWrite{};
			uboWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			uboWrite.dstSet = m_descriptorSets[i];
			uboWrite.dstBinding = 0;
			uboWrite.dstArrayElement = 0;
			uboWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			uboWrite.descriptorCount = 1;
			uboWrite.pBufferInfo = &bufferInfo;

			// Textures
			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = VulkanAPI::Get().Model().Texture().Image().ImageView();
			imageInfo.sampler = VulkanAPI::Get().Model().Texture().Sampler();

			VkWriteDescriptorSet samplerWrite{};
			samplerWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			samplerWrite.dstSet = m_descriptorSets[i];
			samplerWrite.dstBinding = 1;
			samplerWrite.dstArrayElement = 0;
			samplerWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			samplerWrite.descriptorCount = 1;
			samplerWrite.pImageInfo = &imageInfo;

			std::array<VkWriteDescriptorSet, 2> descriptorWrites { uboWrite, samplerWrite };

			vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}
	}
}
