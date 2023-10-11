#include "precompiled.h"
#include "VKVertexArray.h"
#include "VK.h"

namespace Eklipse
{
	namespace Vulkan
	{
		VKVertexArray::VKVertexArray()
		{
		}
		VKVertexArray::~VKVertexArray()
		{
		}
		void VKVertexArray::Bind() const
		{
			for (auto& vertexBuffer : m_vertexBuffers)
				vertexBuffer->Bind();
			m_indexBuffer->Bind();
		}
		void VKVertexArray::Unbind() const
		{
		}
		void VKVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
		{
			EK_ASSERT(vertexBuffer->GetBufferLayout().GetElements().size(), "Vertex Buffer has no layout!");

			uint32_t vertexElementsCounter = 0;
			const auto& layout = vertexBuffer->GetBufferLayout();
			for (const auto& element : layout)
			{
				vertexElementsCounter += element.GetComponentCount();
			}

			m_totalNumVertices += vertexBuffer->GetCount() / vertexElementsCounter;
			m_vertexBuffers.push_back(vertexBuffer);
		}
		void VKVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
		{
			//indexBuffer->Bind();
			m_indexBuffer = indexBuffer;
		}
	}
}
