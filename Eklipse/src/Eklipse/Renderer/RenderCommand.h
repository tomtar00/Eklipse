#pragma once
#include "GraphicsAPI.h"
#include "Shader.h"
#include "VertexArray.h"
#include "Material.h"

namespace Eklipse
{
	class EK_API RenderCommand
	{
		friend class Renderer;

	public:
		static void DrawIndexed(Ref<VertexArray> vertexArray);
		static void DrawIndexed(Ref<VertexArray> vertexArray, Material* texture);

	private:
		static Unique<GraphicsAPI> API;

	};
}