#pragma once
#include <Eklipse/Renderer/Viewport.h>

namespace Eklipse
{
	namespace OpenGL
	{
		class GLViewport : public Eklipse::Viewport
		{
		public:
			GLViewport();
			~GLViewport();

			virtual void Bind() override;
			virtual void Unbind() override;
			virtual void Resize(uint32_t width, uint32_t height) override;

		protected:
			virtual void DrawViewport() override;
			virtual void DrawFullscreen() override;

		private:
			uint32_t m_resolvedId;
			uint32_t m_blitId;
		};
	}
}