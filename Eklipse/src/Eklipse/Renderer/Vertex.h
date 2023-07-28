#pragma once
#include <glm/glm.hpp>

namespace Eklipse
{
	struct Vertex
	{
		glm::vec2 pos;
		glm::vec3 color;

		Vertex(glm::vec2 pos, glm::vec3 color) : pos(pos), color(color) {}
	};
}