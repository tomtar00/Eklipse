#include "precompiled.h"
#include "GLViewport.h"
#include <glad/glad.h>

namespace Eklipse
{
	namespace OpenGL
	{
		GLViewport::GLViewport()
		{
			glGenTextures(1, &m_resolvedId);
			glBindTexture(GL_TEXTURE_2D, m_resolvedId);
		}
		GLViewport::~GLViewport()
		{
		}
		void GLViewport::Bind()
		{
			m_framebuffer->Bind();
		}
		void GLViewport::Unbind()
		{
			m_framebuffer->Unbind();
		}
		void GLViewport::Resize(uint32_t width, uint32_t height)
		{
			m_framebuffer->Resize(width, height);
		}
		void GLViewport::DrawViewport()
		{
		}
		void GLViewport::DrawFullscreen()
		{
		}
	}
}
