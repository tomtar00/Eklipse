#pragma once
#include <vulkan/vulkan.h>
#include <Eklipse/Scene/Scene.h>

namespace Eklipse
{
	namespace Vulkan
	{
		/*Vec<VkVertexInputBindingDescription> GetVertexBindingDescription();
		Vec<VkVertexInputAttributeDescription> GetVertexAttributeDescriptions();

		Vec<VkVertexInputBindingDescription> GetParticleBindingDescription();
		Vec<VkVertexInputAttributeDescription> GetParticleAttributeDescriptions();*/

		VkDescriptorSetLayout CreateDescriptorSetLayout(Vec<VkDescriptorSetLayoutBinding> bindings);
		VkDescriptorPool CreateDescriptorPool(Vec<VkDescriptorPoolSize> poolSizes, int maxSets, VkDescriptorPoolCreateFlags flags);
	}
}