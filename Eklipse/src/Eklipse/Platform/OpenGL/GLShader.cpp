#include "precompiled.h"
#include "GLShader.h"

#include <glm/gtc/type_ptr.hpp>
#include "glm/ext.hpp"
#include <Eklipse/Utils/File.h>

#include <filesystem>
#include <fstream>
#include <shaderc/shaderc.hpp>
#include <spirv_cross.hpp>
#include <spirv_glsl.hpp>
#include <Eklipse/Core/Timer.h>

namespace Eklipse
{
	namespace OpenGL
	{
		static GLenum ShaderTypeFromString(const std::string& stage)
		{
			if (stage == "vertex")
				return GL_VERTEX_SHADER;
			if (stage == "fragment")
				return GL_FRAGMENT_SHADER;

			EK_ASSERT(false, "Unknown shader stage!");
			return 0;
		}

		static shaderc_shader_kind GLShaderStageToShaderC(GLenum stage)
		{
			switch (stage)
			{
			case GL_VERTEX_SHADER:   return shaderc_glsl_vertex_shader;
			case GL_FRAGMENT_SHADER: return shaderc_glsl_fragment_shader;
			}
			EK_ASSERT(false, "Unkown shader stage!");
			return (shaderc_shader_kind)0;
		}

		static const char* GLShaderStageToString(GLenum stage)
		{
			switch (stage)
			{
			case GL_VERTEX_SHADER:   return "GL_VERTEX_SHADER";
			case GL_FRAGMENT_SHADER: return "GL_FRAGMENT_SHADER";
			}
			EK_ASSERT(false, "Unknown shader stage!");
			return nullptr;
		}

		static const char* GetCacheDirectory()
		{
			return "Assets/Cache/Shader/Opengl";
		}

		static void CreateCacheDirectoryIfNeeded()
		{
			std::string cacheDirectory = GetCacheDirectory();
			if (!std::filesystem::exists(cacheDirectory))
				std::filesystem::create_directories(cacheDirectory);
		}

		static const char* GLShaderStageCachedOpenGLFileExtension(uint32_t stage)
		{
			switch (stage)
			{
			case GL_VERTEX_SHADER:    return ".cached_opengl.vert";
			case GL_FRAGMENT_SHADER:  return ".cached_opengl.frag";
			}
			EK_ASSERT(false, "Unknown shader stage!");
			return "";
		}

		static const char* GLShaderStageCachedVulkanFileExtension(uint32_t stage)
		{
			switch (stage)
			{
			case GL_VERTEX_SHADER:    return ".cached_vulkan.vert";
			case GL_FRAGMENT_SHADER:  return ".cached_vulkan.frag";
			}
			EK_ASSERT(false, "Unknown shader stage!");
			return "";
		}

		std::unordered_map<GLenum, std::string> GLShader::PreProcess(const std::string& source)
		{
			EK_PROFILE();

			std::unordered_map<GLenum, std::string> shaderSources;

			const char* typeToken = "#stage";
			size_t typeTokenLength = strlen(typeToken);
			size_t pos = source.find(typeToken, 0);
			while (pos != std::string::npos)
			{
				size_t eol = source.find_first_of("\r\n", pos);
				EK_ASSERT(eol != std::string::npos, "Shader stage syntax error (1)");
				size_t begin = pos + typeTokenLength + 1;
				std::string type = source.substr(begin, eol - begin);
				EK_ASSERT(ShaderTypeFromString(type), "Invalid shader stage specified");

				size_t nextLinePos = source.find_first_not_of("\r\n", eol);
				EK_ASSERT(nextLinePos != std::string::npos, "Shader stage syntax error (2)");
				pos = source.find(typeToken, nextLinePos);

				shaderSources[ShaderTypeFromString(type)] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
			}

			return shaderSources;
		}

		void GLShader::CompileOrGetVulkanBinaries(const std::unordered_map<GLenum, std::string>& shaderSources)
		{
			GLuint program = glCreateProgram();

			shaderc::Compiler compiler;
			shaderc::CompileOptions options;
			options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);
			options.SetOptimizationLevel(shaderc_optimization_level_performance);

			std::filesystem::path cacheDirectory = GetCacheDirectory();

			auto& shaderData = m_vulkanSPIRV;
			shaderData.clear();
			for (auto&& [stage, source] : shaderSources)
			{
				std::filesystem::path shaderFilePath = m_filePath;
				std::filesystem::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() + GLShaderStageCachedVulkanFileExtension(stage));

				std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
				if (in.is_open())
				{
					in.seekg(0, std::ios::end);
					auto size = in.tellg();
					in.seekg(0, std::ios::beg);

					auto& data = shaderData[stage];
					data.resize(size / sizeof(uint32_t));
					in.read((char*)data.data(), size);
				}
				else
				{
					shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, GLShaderStageToShaderC(stage), m_filePath.c_str(), options);
					if (module.GetCompilationStatus() != shaderc_compilation_status_success)
					{
						EK_ASSERT(false, "{0}", module.GetErrorMessage());
					}

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

			for (auto&& [stage, data] : shaderData)
				Reflect(stage, data);
		}

		void GLShader::CompileOrGetOpenGLBinaries()
		{
			auto& shaderData = m_openGLSPIRV;

			shaderc::Compiler compiler;
			shaderc::CompileOptions options;
			options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
			options.SetOptimizationLevel(shaderc_optimization_level_performance);

			std::filesystem::path cacheDirectory = GetCacheDirectory();

			shaderData.clear();
			m_openGLSourceCode.clear();
			for (auto&& [stage, spirv] : m_vulkanSPIRV)
			{
				std::filesystem::path shaderFilePath = m_filePath;
				std::filesystem::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() + GLShaderStageCachedOpenGLFileExtension(stage));

				std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
				if (in.is_open())
				{
					in.seekg(0, std::ios::end);
					auto size = in.tellg();
					in.seekg(0, std::ios::beg);

					auto& data = shaderData[stage];
					data.resize(size / sizeof(uint32_t));
					in.read((char*)data.data(), size);
				}
				else
				{
					spirv_cross::CompilerGLSL glslCompiler(spirv);
					m_openGLSourceCode[stage] = glslCompiler.compile();
					auto& source = m_openGLSourceCode[stage];

					shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, GLShaderStageToShaderC(stage), m_filePath.c_str());
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
				GLuint shaderID = shaderIDs.emplace_back(glCreateShader(stage));
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
				EK_CORE_ERROR("Shader linking failed ({0}):\n{1}", m_filePath, infoLog.data());

				glDeleteProgram(program);

				for (auto id : shaderIDs)
					glDeleteShader(id);
			}

			for (auto id : shaderIDs)
			{
				glDetachShader(program, id);
				glDeleteShader(id);
			}

			m_id = program;
		}

		void GLShader::Reflect(GLenum stage, const std::vector<uint32_t>& shaderData)
		{
			spirv_cross::Compiler compiler(shaderData);
			spirv_cross::ShaderResources resources = compiler.get_shader_resources();

			EK_CORE_TRACE("OpenGLShader::Reflect - {0} {1}", GLShaderStageToString(stage), m_filePath);
			EK_CORE_TRACE("    {0} uniform buffers", resources.uniform_buffers.size());
			EK_CORE_TRACE("    {0} resources", resources.sampled_images.size());

			EK_CORE_TRACE("Uniform buffers:");
			for (const auto& resource : resources.uniform_buffers)
			{
				const auto& bufferType = compiler.get_type(resource.base_type_id);
				uint32_t bufferSize = compiler.get_declared_struct_size(bufferType);
				uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
				int memberCount = bufferType.member_types.size();

				EK_CORE_TRACE("  {0}", resource.name);
				EK_CORE_TRACE("    Size = {0}", bufferSize);
				EK_CORE_TRACE("    Binding = {0}", binding);
				EK_CORE_TRACE("    Members = {0}", memberCount);
			}
		}

		GLShader::GLShader(const std::string& filePath) : m_id(0)
		{
			CreateCacheDirectoryIfNeeded();

			std::string source = ReadFileFromPath(filePath);
			auto shaderSources = PreProcess(source);

			{
				Timer timer;
				CompileOrGetVulkanBinaries(shaderSources);
				CompileOrGetOpenGLBinaries();
				CreateProgram();
				EK_CORE_WARN("Shader creation took {0} ms", timer.ElapsedTimeMs());
			}

			// Extract name from filepath
			auto lastSlash = filePath.find_last_of("/\\");
			lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
			auto lastDot = filePath.rfind('.');
			auto count = lastDot == std::string::npos ? filePath.size() - lastSlash : lastDot - lastSlash;
			m_name = filePath.substr(lastSlash, count);
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
