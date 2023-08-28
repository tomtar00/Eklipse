#pragma once

namespace Eklipse
{
	namespace OpenGL
	{
		struct ViewportSize
		{
			uint32_t width, height;
		};

		extern ViewportSize g_viewportSize;
		extern void* g_viewportTexture;
	}
}