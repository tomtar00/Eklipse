#pragma once
#include "GLFramebuffer.h"

namespace Eklipse
{
	namespace OpenGL
	{
		//extern GLFramebuffer* g_GLDefaultFramebuffer;
		extern Vec<GLFramebuffer*> g_GLOffScreenFramebuffers;
	}
}