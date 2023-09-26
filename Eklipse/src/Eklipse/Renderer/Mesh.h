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

		inline const Ref<VertexArray>& GetVertexArray() const { return m_vertexArray; }
		inline const Ref<Texture>& GetTexture() const { return m_texture; }

	private:
		Ref<VertexArray> m_vertexArray;
		Ref<Texture> m_texture;
	};
}