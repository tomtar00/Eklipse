#pragma once
#include "VertexArray.h"
#include "Texture.h"

namespace Eklipse
{
	class Mesh
	{
	public:
		Mesh() = default;
		Mesh(const char* modelPath, const char* texturePath);

	private:
		Ref<VertexArray> m_vertexArray;
		Ref<Texture> m_texture;
	};
}