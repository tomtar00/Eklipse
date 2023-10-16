#pragma once

#include <Eklipse/Renderer/VertexArray.h>

namespace Eklipse
{
	namespace Vulkan
	{
		class VKVertexArray : public Eklipse::VertexArray
		{
		public:
			VKVertexArray();
			virtual ~VKVertexArray() = default;

			virtual void Bind() const override;
			virtual void Unbind() const override;
			virtual void Dispose() const override;

			virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) override;
			virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) override;
		};
	}
}