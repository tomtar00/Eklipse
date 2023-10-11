#pragma once
#include "VertexArray.h"

namespace Eklipse
{
	class Mesh
	{
	public:
		Mesh() = default;
		Mesh(const char* modelPath);

		inline const Ref<VertexArray>& GetVertexArray() const { return m_vertexArray; }

	private:
		Ref<VertexArray> m_vertexArray;
	};
}