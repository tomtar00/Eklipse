#include "precompiled.h"
#include "GLMaterial.h"
#include "GLShader.h"

namespace Eklipse
{
	namespace OpenGL
	{
		GLMaterial::GLMaterial(Ref<Shader> shader) : Material(shader)
		{
			// TODO: Implement
		}
		GLMaterial::~GLMaterial()
		{
		}
		void GLMaterial::Bind()
		{
			m_shader->Bind();
		}
	}
}
