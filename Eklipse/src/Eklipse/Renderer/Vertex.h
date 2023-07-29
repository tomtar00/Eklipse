#pragma once
#include <glm/glm.hpp>

namespace Eklipse
{
	struct Vertex
	{
		glm::vec2 pos;
		glm::vec3 color;
		glm::vec2 texCoord;

		Vertex(glm::vec2 pos, glm::vec3 color, glm::vec2 texCoord) 
			: pos(pos), color(color), texCoord(texCoord) {}
	};
}