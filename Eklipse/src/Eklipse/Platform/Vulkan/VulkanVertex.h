#pragma once
#include <vulkan/vulkan.h>
#include <Eklipse/Renderer/Vertex.h>

namespace Eklipse
{
	struct VulkanVertex : Vertex
	{
		VulkanVertex(glm::vec2 pos, glm::vec3 color) : Vertex(pos, color) {}

		static VkVertexInputBindingDescription GetBindingDescription();
		static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions();
	};
}