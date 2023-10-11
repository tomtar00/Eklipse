#include "precompiled.h"
#include "RenderCommand.h"

#include <Eklipse/Utils/Stats.h>

namespace Eklipse
{
	Unique<GraphicsAPI> RenderCommand::API;

	void RenderCommand::DrawIndexed(Ref<VertexArray> vertexArray)
	{
		EK_PROFILE();

		vertexArray->Bind();
		API->DrawIndexed(vertexArray);

		Stats::Get().drawCalls++;
		Stats::Get().numVertices += vertexArray->GetTotalNumVertices();
	}
	void RenderCommand::DrawIndexed(Ref<VertexArray> vertexArray, Material* material)
	{
		EK_PROFILE();

		material->Bind();
		DrawIndexed(vertexArray);
	}
}