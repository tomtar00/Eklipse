#include "precompiled.h"
#include "RenderCommand.h"

#include <Eklipse/Utils/Stats.h>

namespace Eklipse
{
	Unique<GraphicsAPI> RenderCommand::API;

	void RenderCommand::DrawIndexed(Ref<Shader> shader, Ref<VertexArray> vertexArray)
	{
		shader->Bind();
		vertexArray->Bind();

		API->DrawIndexed(vertexArray);

		Stats::Get().drawCalls++;
		for (auto& vb : vertexArray->GetVertexBuffers())
		{
			Stats::Get().numVertices += vb->GetCount();
		}
	}
	void RenderCommand::DrawIndexed(Ref<Shader> shader, Ref<VertexArray> vertexArray, Ref<Texture> texture)
	{
		texture->Bind();
		DrawIndexed(shader, vertexArray);
	}
}