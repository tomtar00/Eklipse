#pragma once

#include "Framebuffer.h"

namespace Eklipse
{
	class Viewport
	{
	public:
		static Ref<Viewport> Create();
		virtual ~Viewport() = default;

		void Draw();
		virtual void Bind()= 0;
		virtual void Unbind() = 0;
		virtual void Resize(uint32_t width, uint32_t height) = 0;

	protected:
		virtual void DrawViewport() = 0;
		virtual void DrawFullscreen() = 0;

	protected:
		Ref<Framebuffer> m_framebuffer;
	};
}