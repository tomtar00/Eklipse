#pragma once
#include "GraphicsAPI.h"
#include "Shader.h"
#include "VertexArray.h"
#include "Texture.h"

namespace Eklipse
{
	class RenderCommand
	{
	public:
		static void DrawIndexed(Ref<VertexArray> vertexArray);
		static void DrawIndexed(Ref<VertexArray> vertexArray, Ref<Texture> texture);

		static Unique<GraphicsAPI> API;
	};
}