#include "precompiled.h"
#include "GLMaterial.h"
#include "GLShader.h"

namespace Eklipse
{
	namespace OpenGL
	{
		GLMaterial::GLMaterial(const Path& path, AssetHandle shaderHandle) : Material(path, shaderHandle)
		{
			m_glShader = std::static_pointer_cast<GLShader>(m_shader);
		}
		void GLMaterial::Bind()
		{
			EK_PROFILE();
			Material::Bind();

			for (auto&& [stage, reflection] : m_shader->GetReflections())
			{
				for (auto& pushConstant : reflection.pushConstants)
				{
					for (auto& member : pushConstant.members)
					{
						String constantName = pushConstant.name + "." + member.name;
						GLint location = glGetUniformLocation(m_glShader->GetID(), constantName.c_str());
						EK_ASSERT(location != -1, "Uniform {0} not found in shader '{1}' or the variable is not used", constantName, m_shader->Name);
						if (location == -1) continue;

						void* data = m_pushConstants.at(pushConstant.name).dataPointers.at(member.name).data;
						switch (member.type)
						{
							//case DataType::BOOL:	glUniform1iv(location, 1, (int*)data);						break;
							case ShaderDataType::INT:		glUniform1iv(location, 1, (int*)data);						break;
							case ShaderDataType::INT2:		glUniform2iv(location, 1, (int*)data);						break;
							case ShaderDataType::INT3:		glUniform3iv(location, 1, (int*)data);						break;
							case ShaderDataType::INT4:		glUniform4iv(location, 1, (int*)data);						break;
							case ShaderDataType::FLOAT :	glUniform1fv(location, 1, (float*)data);					break;
							case ShaderDataType::FLOAT2:	glUniform2fv(location, 1, (float*)data);					break;
							case ShaderDataType::FLOAT3:	glUniform3fv(location, 1, (float*)data);					break;
							case ShaderDataType::FLOAT4:	glUniform4fv(location, 1, (float*)data);					break;
							case ShaderDataType::MAT3:		glUniformMatrix3fv(location, 1, GL_FALSE, (float*)data);	break;
							case ShaderDataType::MAT4:		glUniformMatrix4fv(location, 1, GL_FALSE, (float*)data);	break;
							default: EK_CORE_ERROR("Unsupported uniform type: {0}", (int)member.type);
						}
					};
				}

				uint32_t index = 0;
				for (auto&& [samplerName, sampler] : m_samplers)
				{
					if (sampler.texture == nullptr) 
						continue;

					GLint textureUniformLocation = glGetUniformLocation(m_glShader->GetID(), samplerName.c_str());
					glUniform1i(textureUniformLocation, sampler.binding);
					glActiveTexture(GL_TEXTURE0 + sampler.binding);
					sampler.texture->Bind();
				}
			}
		}
		void GLMaterial::Dispose()
		{	
		}
	}
}