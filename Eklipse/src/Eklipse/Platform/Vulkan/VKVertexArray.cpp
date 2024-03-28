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
		void VKVertexArray::Bind() const
		{
			EK_CORE_PROFILE();
			for (auto& vertexBuffer : m_vertexBuffers)
				vertexBuffer->Bind();
			if (m_indexBuffer)
				m_indexBuffer->Bind();
		}
		void VKVertexArray::Unbind() const
		{
		}
		void VKVertexArray::Dispose() const
		{
			EK_CORE_PROFILE();
			for (auto& vertexBuffer : m_vertexBuffers)
				vertexBuffer->Dispose();
			if (m_indexBuffer)
				m_indexBuffer->Dispose();
		}
		void VKVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
		{
			EK_CORE_PROFILE();
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
			m_indexBuffer = indexBuffer;
		}
	}
}
