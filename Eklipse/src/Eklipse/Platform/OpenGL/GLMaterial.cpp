#include "precompiled.h"
#include "GLMaterial.h"
#include "GLShader.h"

#include <Eklipse/Scene/Assets.h>

namespace Eklipse
{
	namespace OpenGL
	{
		GLMaterial::GLMaterial(const std::filesystem::path& path) : Material(path)
		{
			m_glShader = std::static_pointer_cast<GLShader>(m_shader);
		}
		void GLMaterial::Bind()
		{
			Material::Bind();

			for (auto&& [stage, reflection] : m_shader->GetReflections())
			{
				for (auto& pushConstant : reflection.pushConstants)
				{
					for (auto& member : pushConstant.members)
					{
						std::string constantName = pushConstant.name + "." + member.name;
						GLint location = glGetUniformLocation(m_glShader->GetID(), constantName.c_str());
						EK_ASSERT(location != -1, "Uniform {0} not found in shader '{1}'", constantName, m_shader->GetName());

						float* data = static_cast<float*>(m_pushConstants[pushConstant.name].dataPointers[member.name].data);
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

				uint32_t index = 0;
				for (auto& sampler : reflection.samplers)
				{
					if (m_sampledTextures.size() <= index) break;
					glActiveTexture(GL_TEXTURE0 + sampler.binding);
					m_sampledTextures[index++]->Bind();
				}
			}
		}
		void GLMaterial::Dispose()
		{
			Material::Dispose();
		}
	}
}