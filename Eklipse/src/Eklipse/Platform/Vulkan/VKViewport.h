#pragma once
#include <Eklipse/Renderer/Viewport.h>

namespace Eklipse
{
	namespace Vulkan
	{
		class VKViewport : public Eklipse::Viewport
		{
		public:
			VKViewport(ViewportCreateInfo& info);
			~VKViewport();

			virtual void BindFramebuffer() override;
			virtual void UnbindFramebuffer() override;
			virtual void Resize(uint32_t width, uint32_t height) override;
			virtual void Bind() override;

			virtual Ref<VertexArray> GetVertexArray() const override;
		};
	}
}