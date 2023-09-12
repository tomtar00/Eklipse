#pragma once
#include <Eklipse/Renderer/Viewport.h>
#include "GLFramebuffer.h"

namespace Eklipse
{
	namespace OpenGL
	{
		class GLViewport : public Eklipse::Viewport
		{
		public:
			GLViewport(ViewportCreateInfo& info);

			virtual void Bind() override;
			virtual void Unbind() override;
			virtual void Resize(uint32_t width, uint32_t height) override;

		protected:
			virtual void DrawViewport() override;
			virtual void DrawFullscreen() override;

		private:
			Ref<GLFramebuffer> m_blitFramebuffer;
		};
	}
}