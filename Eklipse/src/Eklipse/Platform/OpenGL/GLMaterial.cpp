#include "precompiled.h"
#include "GLMaterial.h"
#include "GLShader.h"

#include <Eklipse/Scene/Assets.h>
#include "GLTexture.h"

namespace Eklipse
{
	namespace OpenGL
	{
		Ref<Texture2D> s_texture;

		GLMaterial::GLMaterial(Ref<Shader> shader) : Material(shader)
		{
			m_glShader = std::static_pointer_cast<GLShader>(shader);
			s_texture = Assets::GetTexture("Assets/Textures/viking_room.png");
		}
		void GLMaterial::Bind()
		{
			Material::Bind();

			glActiveTexture(GL_TEXTURE0 + m_shader->GetFragmentReflection().samplers[0].binding);
			s_texture->Bind();

			for (auto&& [stage, reflection] : m_shader->GetReflections())
			{
				for (auto& pushConstant : reflection.pushConstants)
				{
					for (auto& member : pushConstant.members)
					{
						std::string constantName = pushConstant.name + "." + member.name;
						GLint location = glGetUniformLocation(m_glShader->GetID(), constantName.c_str());
						EK_ASSERT(location != -1, "Uniform {0} not found in shader '{1}'", constantName, m_shader->GetName());

						float* data = (float*)m_pushConstants[pushConstant.name].dataPointers[member.name].data;
						switch (pushConstant.size)
						{
							case sizeof(float) :	 glUniform1fv(location, 1, (float*)data); break;
							case sizeof(glm::vec2) : glUniform2fv(location, 1, (float*)data); break;
							case sizeof(glm::vec3) : glUniform3fv(location, 1, (float*)data); break;
							case sizeof(glm::vec4) : glUniform4fv(location, 1, (float*)data); break;
							case sizeof(glm::mat3) : glUniformMatrix3fv(location, 1, GL_FALSE, (float*)data); break;
							case sizeof(glm::mat4) : glUniformMatrix4fv(location, 1, GL_FALSE, (float*)data); break;
							default: EK_ASSERT(false, "Unsupported uniform size: {0}", pushConstant.size);
						}
					};
				}
			}
		}
		void GLMaterial::Dispose()
		{
			Material::Dispose();
		}
	}
}