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
		void CreateDescriptorSetUniformLayout();

	private:
		VkDescriptorSetLayout m_descriptorSetUniformLayout;
	};

	class VulkanDescriptorPool
	{
	public:
		void Init(uint32_t descriptorCount);
		void Shutdown();

		std::vector<VkDescriptorSet>& DescriptorSets();

	private:
		void CreateDescriptorUniformPool(uint32_t descriptorCount);
		void CreateDescriptorUniformSets(uint32_t descriptorCount);

	private:
		VkDescriptorPool m_descriptorUniformPool;
		std::vector<VkDescriptorSet> m_descriptorSets;
	};
}