#include "precompiled.h"
#include "RenderCommand.h"

#include <Eklipse/Utils/Stats.h>
#include <Eklipse/Renderer/Renderer.h>

namespace Eklipse
{
	Unique<GraphicsAPI> RenderCommand::API;

	void RenderCommand::Draw(Ref<VertexArray> vertexArray)
	{
		EK_PROFILE();
		EK_ASSERT(vertexArray, "Vertex array is null");

		vertexArray->Bind();
		API->Draw(vertexArray);

		Stats::Get().drawCalls++;
		Stats::Get().numVertices += vertexArray->GetTotalNumVertices();
	}
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
		config.type				= Renderer::GetSettings().PipelineType;
		config.topologyMode		= Renderer::GetSettings().PipelineTopologyMode;
		config.shader			= material->GetShader().get();
		config.framebuffer		= g_currentFramebuffer;
		config.depthTest		= true;
		Pipeline::Get(config)->Bind();

		material->Bind();
		DrawIndexed(vertexArray);
	}
	void RenderCommand::DrawLines(Ref<VertexArray> vertexArray, Material* material)
	{
		EK_PROFILE();
		EK_ASSERT(material, "Material is null");
		EK_ASSERT(material->GetShader(), "Material shader is null");
		EK_ASSERT(g_currentFramebuffer, "Current framebuffer is null");

		Pipeline::Config config{};
		config.type			= Pipeline::Type::Rasterization;
		config.topologyMode = Pipeline::TopologyMode::Line;
		config.shader		= material->GetShader().get();
		config.framebuffer	= g_currentFramebuffer;
		config.depthTest    = false;
		Pipeline::Get(config)->Bind();

		material->Bind();
		Draw(vertexArray);
	}
	void RenderCommand::Dispatch(Ref<ComputeShader> computeShader, uint32_t x, uint32_t y, uint32_t z)
	{
        EK_PROFILE();
        EK_ASSERT(computeShader, "Compute shader is null");

		Pipeline::Config config{};
		config.type				= Pipeline::Type::Compute;
		config.topologyMode		= Pipeline::TopologyMode::Triangle;
		config.shader			= computeShader->GetShader().get();
		config.framebuffer		= g_currentFramebuffer; // at dispatch time there is no framebuffer bound, so this is null
		Pipeline::Get(config)->Bind();

		computeShader->GetMaterial()->Bind();
        computeShader->Dispatch(x, y, z);
	}
}