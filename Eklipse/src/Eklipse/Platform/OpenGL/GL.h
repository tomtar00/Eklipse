#pragma once
#include "GLFramebuffer.h"

namespace Eklipse
{
	namespace OpenGL
	{
		//extern GLFramebuffer* g_GLDefaultFramebuffer;
		extern std::vector<GLFramebuffer*> g_GLOffScreenFramebuffers;
	}
}