#pragma once
#include <glm/glm.hpp>

// TODO: Remove this files (moved from struct to BufferLayout)

namespace Eklipse
{
	struct EK_API Vertex
	{
		glm::vec3 pos;
		glm::vec3 color;
		glm::vec2 texCoord;

		bool operator==(const Vertex& other) const;
	};

	struct Particle
	{
		glm::vec3 pos;
		glm::vec3 vel;
		glm::vec4 color;
	};
}