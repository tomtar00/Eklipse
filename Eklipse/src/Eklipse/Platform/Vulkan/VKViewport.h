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

			virtual void Bind() override;
			virtual void Unbind() override;
			virtual void Resize(uint32_t width, uint32_t height) override;

		protected:
			virtual void DrawViewport() override;
			virtual void DrawFullscreen() override;
		};
	}
}