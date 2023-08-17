#pragma once
#include <vulkan/vulkan.h>
#include <Eklipse/Scene/Scene.h>

namespace Eklipse
{
	namespace Vulkan
	{
		std::vector<VkVertexInputBindingDescription> GetVertexBindingDescription();
		std::vector<VkVertexInputAttributeDescription> GetVertexAttributeDescriptions();

		std::vector<VkVertexInputBindingDescription> GetParticleBindingDescription();
		std::vector<VkVertexInputAttributeDescription> GetParticleAttributeDescriptions();

		VkDescriptorSetLayout CreateDescriptorSetLayout(std::vector<VkDescriptorSetLayoutBinding> bindings);
		VkDescriptorPool CreateDescriptorPool(std::vector<VkDescriptorPoolSize> poolSizes, int maxSets);
	}
}