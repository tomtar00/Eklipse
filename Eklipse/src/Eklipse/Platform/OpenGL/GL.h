#pragma once
#include "GLFramebuffer.h"

namespace Eklipse
{
	namespace OpenGL
	{
		struct ViewportSize
		{
			uint32_t width, height;
		};

		extern ViewportSize g_viewportSize;
		extern uint32_t g_viewportTexture;
		extern GLFramebuffer* g_framebuffer;
	}
}