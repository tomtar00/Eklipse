#pragma once
#include "VertexArray.h"

namespace Eklipse
{
	class Mesh
	{
	public:
		Mesh() = delete;
		Mesh(std::vector<float> vertices, std::vector<uint32_t> indices);

		inline const Ref<VertexArray>& GetVertexArray() const { return m_vertexArray; }

	private:
		Ref<VertexArray> m_vertexArray;
	};
}