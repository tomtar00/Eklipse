#pragma once
#include "GraphicsAPI.h"
#include "Shader.h"
#include "VertexArray.h"
#include "Material.h"
#include "ComputeShader.h"

namespace Eklipse
{
	class EK_API RenderCommand
	{
		friend class Renderer;

	public:
		static void Draw(Ref<VertexArray> vertexArray);
		static void DrawIndexed(Ref<VertexArray> vertexArray);
		static void DrawIndexed(Ref<VertexArray> vertexArray, Material* material);
		static void DrawLines(Ref<VertexArray> vertexArray, Material* material);

		static void Dispatch(Ref<ComputeShader> computeShader, uint32_t x, uint32_t y, uint32_t z);

	private:
		static Unique<GraphicsAPI> API;
	};
}