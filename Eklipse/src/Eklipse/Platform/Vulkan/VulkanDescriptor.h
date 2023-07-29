#pragma once
#include <vulkan/vulkan.h>

namespace Eklipse
{
	class VulkanDescriptorSetLayout
	{
	public:
		void Init();
		void Shutdown();

		VkDescriptorSetLayout& UniformLayout();

	private:
		void CreateDescriptorSetLayout();

	private:
		VkDescriptorSetLayout m_uniformDescriptorSetLayout;
	};

	class VulkanDescriptorPool
	{
	public:
		void Init(uint32_t descriptorCount);
		void Shutdown();

		std::vector<VkDescriptorSet>& DescriptorSets();

	private:
		void CreateDescriptorPool(uint32_t descriptorCount);
		void CreateUniformDescriptorSets(uint32_t descriptorCount);

	private:
		VkDescriptorPool m_descriptorPool;
		std::vector<VkDescriptorSet> m_descriptorSets;
	};
}