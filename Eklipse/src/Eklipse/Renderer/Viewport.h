#pragma once

#include "Framebuffer.h"

namespace Eklipse
{
	enum ViewportFlags
	{
		VIEWPORT_NONE				= 0, 
		VIEWPORT_BLIT_FRAMEBUFFER	= BIT(1),
		VIEWPORT_FULLSCREEN			= BIT(2)
	};
	struct ViewportCreateInfo
	{
		ViewportFlags flags;
		FramebufferInfo framebufferInfo;
	};

	class Viewport
	{
	public:
		static Ref<Viewport> Create(ViewportCreateInfo& info);

		Viewport(ViewportCreateInfo& info) : m_createInfo(info) {}
		virtual ~Viewport() = default;

		void Draw();
		virtual void Bind()= 0;
		virtual void Unbind() = 0;
		virtual void Resize(uint32_t width, uint32_t height) = 0;

	protected:
		virtual void DrawViewport() = 0;
		virtual void DrawFullscreen() = 0;

	protected:
		ViewportCreateInfo m_createInfo;
		Ref<Framebuffer> m_framebuffer;
	};
}