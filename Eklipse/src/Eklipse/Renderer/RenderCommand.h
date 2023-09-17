#pragma once
#include "GraphicsAPI.h"
#include "Shader.h"

namespace Eklipse
{
	class RenderCommand
	{
	public:
		static void DrawIndexed(Ref<Shader> shader, Ref<VertexArray> vertexArray);
		static void DrawIndexed(Ref<Shader> shader, Ref<VertexArray> vertexArray, Ref<Texture> texture);

		static Unique<GraphicsAPI> API;
	};
}