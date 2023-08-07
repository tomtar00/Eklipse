#pragma once
#include <glm/glm.hpp>

namespace Eklipse
{
	struct Vertex
	{
		glm::vec3 pos;
		glm::vec3 color;
		glm::vec2 texCoord;

		bool Vertex::operator==(const Vertex& other) const;
	};

	struct Particle
	{
		glm::vec3 pos;
		glm::vec3 vel;
		glm::vec4 color;
	};
}