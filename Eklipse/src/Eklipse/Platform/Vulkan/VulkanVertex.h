#pragma once
#include <vulkan/vulkan.h>
#include <Eklipse/Renderer/Vertex.h>

#include <glm/gtx/hash.hpp>

namespace Eklipse
{
	struct VulkanVertex : Vertex
	{
		VulkanVertex() : Vertex(glm::vec3{}, glm::vec3{}, glm::vec2{}) {}
		VulkanVertex(glm::vec3 pos, glm::vec3 color, glm::vec2 texCoord) : Vertex(pos, color, texCoord) {}

		static VkVertexInputBindingDescription GetBindingDescription();
		static std::array<VkVertexInputAttributeDescription, 3> GetAttributeDescriptions();

		bool operator==(const VulkanVertex& other) const;
	};
}

namespace std 
{
	template<> struct hash<Eklipse::VulkanVertex> 
	{
		size_t operator()(Eklipse::VulkanVertex const& vertex) const 
		{
			return ((hash<glm::vec3>()(vertex.pos) ^
				(hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
				(hash<glm::vec2>()(vertex.texCoord) << 1);
		}
	};
}