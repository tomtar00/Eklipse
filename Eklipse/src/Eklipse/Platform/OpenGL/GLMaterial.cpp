#include "precompiled.h"
#include "GLMaterial.h"
#include "GLShader.h"

namespace Eklipse
{
	namespace OpenGL
	{
		GLMaterial::GLMaterial(Ref<Shader> shader) : Material(shader)
		{
			m_glShader = std::static_pointer_cast<GLShader>(shader);
		}
		void GLMaterial::Bind()
		{
			Material::Bind();
		}
		void GLMaterial::Dispose()
		{
			Material::Dispose();
		}
	}
}