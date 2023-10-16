#include "precompiled.h"
#include "GLShader.h"

#include <glm/gtc/type_ptr.hpp>
#include "glm/ext.hpp"
#include <Eklipse/Utils/File.h>

#include <filesystem>
#include <shaderc/shaderc.hpp>
#include <spirv_cross.hpp>
#include <spirv_glsl.hpp>
#include <Eklipse/Core/Timer.h>

namespace Eklipse
{
	namespace OpenGL
	{
		static GLenum GLShaderStageFromInternalStage(const ShaderStage stage)
		{
			switch (stage)
			{
				case ShaderStage::VERTEX:   return GL_VERTEX_SHADER;
				case ShaderStage::FRAGMENT: return GL_FRAGMENT_SHADER;
			}
			EK_ASSERT(false, "Unknown shader stage!");
			return 0;
		}

		static const char* GLShaderStageCachedOpenGLFileExtension(const ShaderStage stage)
		{
			switch (stage)
			{
				case ShaderStage::VERTEX:    return ".cached_opengl.vert";
				case ShaderStage::FRAGMENT:  return ".cached_opengl.frag";
			}
			EK_ASSERT(false, "Unknown shader stage!");
			return "";
		}

		void GLShader::CompileOrGetOpenGLBinaries()
		{
			auto& shaderData = m_openGLSPIRV;

			shaderc::Compiler compiler;
			shaderc::CompileOptions options;
			options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);

			//! Strips reflection info
			//options.SetOptimizationLevel(shaderc_optimization_level_performance);

			std::filesystem::path cacheDirectory = GetCacheDirectoryPath();

			shaderData.clear();
			m_openGLSourceCode.clear();
			for (auto&& [stage, spirv] : m_vulkanSPIRV)
			{
				std::filesystem::path shaderFilePath = m_filePath;
				std::filesystem::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() + GLShaderStageCachedOpenGLFileExtension(stage));

				std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
				if (in.is_open())
				{
					EK_CORE_INFO("Reading OpenGL shader cache binaries from path: '{0}'", m_filePath);

					in.seekg(0, std::ios::end);
					auto size = in.tellg();
					in.seekg(0, std::ios::beg);

					auto& data = shaderData[stage];
					data.resize(size / sizeof(uint32_t));
					in.read((char*)data.data(), size);
				}
				else
				{
					EK_CORE_INFO("Compiling shader at path: '{0}' to OpenGL binaries", m_filePath);

					spirv_cross::CompilerGLSL glslCompiler(spirv);
					m_openGLSourceCode[stage] = glslCompiler.compile();
					auto& source = m_openGLSourceCode[stage];

					shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, (shaderc_shader_kind)ShaderStageToShaderC(stage), m_filePath.c_str());
					EK_ASSERT(module.GetCompilationStatus() == shaderc_compilation_status_success, "{0}", module.GetErrorMessage());
					
					shaderData[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());

					std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
					if (out.is_open())
					{
						auto& data = shaderData[stage];
						out.write((char*)data.data(), data.size() * sizeof(uint32_t));
						out.flush();
						out.close();
					}
				}
			}
		}

		void GLShader::CreateProgram()
		{
			GLuint program = glCreateProgram();

			std::vector<GLuint> shaderIDs;
			for (auto&& [stage, spirv] : m_openGLSPIRV)
			{
				GLuint shaderID = shaderIDs.emplace_back(glCreateShader(GLShaderStageFromInternalStage(stage)));
				glShaderBinary(1, &shaderID, GL_SHADER_BINARY_FORMAT_SPIR_V, spirv.data(), spirv.size() * sizeof(uint32_t));
				glSpecializeShader(shaderID, "main", 0, nullptr, nullptr);
				glAttachShader(program, shaderID);
			}

			glLinkProgram(program);

			GLint isLinked;
			glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
			if (isLinked == GL_FALSE)
			{
				GLint maxLength;
				glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

				std::vector<GLchar> infoLog(maxLength);
				glGetProgramInfoLog(program, maxLength, &maxLength, infoLog.data());
				if (infoLog.data())
					EK_CORE_ERROR("Shader linking failed ({0}): {1}", m_filePath, infoLog.data());
				else
					EK_CORE_ERROR("Shader linking failed ({0}): {1}", m_filePath, "No info log available");

				glDeleteProgram(program);

				for (auto id : shaderIDs)
					glDeleteShader(id);
			}
			else
			{
				for (auto id : shaderIDs)
				{
					glDetachShader(program, id);
					glDeleteShader(id);
				}
			}

			m_id = program;
		}

		GLShader::GLShader(const std::string& filePath) : m_id(0), Shader(filePath)
		{
			auto shaderSources = Setup();

			{
				Timer timer;
				CompileOrGetVulkanBinaries(shaderSources);
				CompileOrGetOpenGLBinaries();
				CreateProgram();
				EK_CORE_WARN("Creation of shader '{0}' took {1} ms", m_name, timer.ElapsedTimeMs());
			}
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
	}
}
