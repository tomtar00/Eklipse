#include "precompiled.h"
#include "GLShader.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include "glm/ext.hpp"
#include <Eklipse/Utils/File.h>

namespace Eklipse
{
	namespace OpenGL
	{
		uint32_t CreateShader(const std::string& shaderPath, uint32_t shaderType)
		{
			GLint success = false;
			char infoLog[512];

			std::vector<char> shaderCode = ReadFileFromPath(shaderPath);
			const char* code = shaderCode.data();

			GLuint vertex = glCreateShader(shaderType);
			glShaderSource(vertex, 1, &code, NULL);
			glCompileShader(vertex);
			glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(vertex, 512, NULL, infoLog);
				EK_ASSERT("Shader compilation failed! Shader path: {0}. Reason: {1}", shaderPath, infoLog);
			};

			return vertex;
		}

		GLShader::GLShader(const std::string& name, const std::string& vertPath, const std::string& fragPath) : m_id(0)
		{
			uint32_t vert = CreateShader(vertPath, GL_VERTEX_SHADER);
			uint32_t frag = CreateShader(fragPath, GL_FRAGMENT_SHADER);

			m_id = glCreateProgram();
			glAttachShader(m_id, vert);
			glAttachShader(m_id, frag);
			glLinkProgram(m_id);

			GLint success;
			char infoLog[512];
			glGetProgramiv(m_id, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(m_id, 512, NULL, infoLog);
				EK_ASSERT("Shader linking failed! Reason: {0}", infoLog);
			}

			glDeleteShader(vert);
			glDeleteShader(frag);
			m_name = name;
		}
		void GLShader::Bind() const
		{
			EK_PROFILE();

			glUseProgram(m_id);
		}
		void GLShader::Unbind() const
		{
			EK_PROFILE();

			glUseProgram(0);
		}
		void GLShader::Dispose() const
		{
			glDeleteProgram(m_id);
		}
		void GLShader::UploadMat4(const std::string& name, const glm::mat4& matrix)
		{
			EK_PROFILE();

			GLint location = glGetUniformLocation(m_id, name.c_str());
			glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
		}
		void GLShader::UploadInt(const std::string& name, const int value)
		{
			EK_PROFILE();

			GLint location = glGetUniformLocation(m_id, name.c_str());
			glUniform1i(location, value);
		}
	}
}
