#pragma once
#include <vulkan/vulkan.h>
#include <Eklipse/Renderer/Vertex.h>

namespace Eklipse
{
	struct VulkanVertex : Vertex
	{
		VulkanVertex(glm::vec2 pos, glm::vec3 color, glm::vec2 texCoord) : Vertex(pos, color, texCoord) {}

		static VkVertexInputBindingDescription GetBindingDescription();
		static std::array<VkVertexInputAttributeDescription, 3> GetAttributeDescriptions();
	};
}