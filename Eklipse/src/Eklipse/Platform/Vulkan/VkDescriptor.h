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

		void SetupDescriptorSetLayouts();
		void DisposeDescriptorSetLayouts();

		void SetupDescriptorPool();
		void DisposeDescriptorPool();
	}
}