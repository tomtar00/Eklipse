#include "precompiled.h"
#include "RenderCommand.h"

#include <Eklipse/Utils/Stats.h>
#include <Eklipse/Renderer/Renderer.h>

namespace Eklipse
{
	Unique<GraphicsAPI> RenderCommand::API;

	void RenderCommand::DrawIndexed(Ref<VertexArray> vertexArray)
	{
		EK_PROFILE();
		EK_ASSERT(vertexArray, "Vertex array is null");

		vertexArray->Bind();
		API->DrawIndexed(vertexArray);

		Stats::Get().drawCalls++;
		Stats::Get().numVertices += vertexArray->GetTotalNumVertices();
	}
	void RenderCommand::DrawIndexed(Ref<VertexArray> vertexArray, Material* material)
	{
		EK_PROFILE();
		EK_ASSERT(material, "Material is null");
		EK_ASSERT(material->GetShader(), "Material shader is null");
		EK_ASSERT(g_currentFramebuffer, "Current framebuffer is null");

		Pipeline::Config config{};
		config.type = Renderer::GetSettings().PipelineType;
		config.mode = Renderer::GetSettings().PipelineMode;
		config.shader = material->GetShader().get();
		config.framebuffer = g_currentFramebuffer;
		Pipeline::Get(config)->Bind();

		material->Bind();
		DrawIndexed(vertexArray);
	}
}