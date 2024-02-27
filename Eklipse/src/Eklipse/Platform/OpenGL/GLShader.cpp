#include "precompiled.h"
#include "GLShader.h"
#include <Eklipse/Core/Timer.h>
#include <Eklipse/Assets/AssetManager.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>
#include <shaderc/shaderc.hpp>
#include <spirv_cross.hpp>
#include <spirv_glsl.hpp>

namespace Eklipse
{
	namespace OpenGL
	{
		static GLenum GLShaderStageFromInternalStage(const ShaderStage stage)
		{
			EK_CORE_PROFILE();
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
			EK_CORE_PROFILE();
			switch (stage)
			{
				case ShaderStage::VERTEX:    return ".cached_opengl.vert";
				case ShaderStage::FRAGMENT:  return ".cached_opengl.frag";
			}
			EK_ASSERT(false, "Unknown shader stage!");
			return "";
		}

		GLShader::GLShader(const String& vertexSource, const String& fragmentSource, const AssetHandle handle) 
			: m_id(0), Shader(vertexSource, fragmentSource, handle)
		{
			EK_CORE_PROFILE();
			Handle = handle;
			Name = "Custom";
			StageSourceMap shaderSources;
			shaderSources[ShaderStage::VERTEX] = vertexSource;
			shaderSources[ShaderStage::FRAGMENT] = fragmentSource;
			Compile(shaderSources);
		}
		GLShader::GLShader(const Path& filePath, const AssetHandle handle) 
			: m_id(0), Shader(filePath, handle)
		{
			EK_CORE_PROFILE();
			Handle = handle;
			Name = filePath.stem().string();
			Compile(filePath);
		}

		uint32_t GLShader::GetID() const
		{
			return m_id;
		}

		void GLShader::Bind() const
		{
			EK_CORE_PROFILE();

#ifdef EK_DEBUG
			if (m_isValid)
#endif
				glUseProgram(m_id);
		}
		void GLShader::Unbind() const
		{
			EK_CORE_PROFILE();

			glUseProgram(0);
		}
		void GLShader::Dispose()
		{
			EK_CORE_PROFILE();
			EK_CORE_TRACE("Disposing OpenGL shader '{0}'", Name);
			glDeleteProgram(m_id);
			EK_CORE_DBG("Disposed OpenGL shader '{0}'", Name);
		}

		bool GLShader::Compile(const Path& shaderPath, bool forceCompile)
		{
			EK_CORE_PROFILE();
			auto shaderSources = Setup(shaderPath);
			return Compile(shaderPath, shaderSources, forceCompile);
		}
		bool GLShader::Compile(const StageSourceMap& sourceMap, bool forceCompile)
		{
			EK_CORE_PROFILE();
			return Compile("", sourceMap, forceCompile);
		}

		bool GLShader::Compile(const Path& shaderPath, const StageSourceMap& sourceMap, bool forceCompile)
		{
			EK_CORE_TRACE("Compiling OpenGL shader '{0}'", Name);
			Timer timer;
			bool success = true;
			success = success && CompileOrGetVulkanBinaries(shaderPath, sourceMap, forceCompile);
			success = success && CompileOrGetOpenGLBinaries(shaderPath, forceCompile);
			if (success)
			{
				Dispose();
				CreateProgram();
				EK_CORE_DBG("Creation of shader '{0}' took {1} ms", Name, timer.ElapsedTimeMs());
			}
			else EK_CORE_ERROR("Failed to compile shader {0}", Handle);
			return success;
		}
		const String GLShader::GetCacheDirectoryPath()
		{
			return "Assets/Cache/Shader/OpenGL";
		}

		bool GLShader::CompileOrGetOpenGLBinaries(const Path& shaderPath, bool forceCompile)
		{
			EK_CORE_PROFILE();
			EK_CORE_TRACE("Compiling or getting binaries for OpenGL shader '{0}'", Name);

			auto& shaderData = m_openGLSPIRV;

			bool success = true;
			shaderc::Compiler compiler;
			shaderc::CompileOptions options;
			options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
			options.SetSourceLanguage(shaderc_source_language_glsl);
			options.SetIncluder(Unique<shaderc::CompileOptions::IncluderInterface>(new ShaderIncluder));

			Path cacheDirectory = GetCacheDirectoryPath();

			shaderData.clear();
			StageSourceMap openGLSourceCode;
			for (auto&& [stage, spirv] : m_vulkanSPIRV)
			{
				Path cachedPath = cacheDirectory / (Name + GLShaderStageCachedOpenGLFileExtension(stage));

				std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
				if (!forceCompile && in.is_open())
				{
					EK_CORE_TRACE("Reading OpenGL shader cache binaries from path: '{0}'", cachedPath.string());

					in.seekg(0, std::ios::end);
					auto size = in.tellg();
					in.seekg(0, std::ios::beg);

					auto& data = shaderData[stage];
					data.resize(size / sizeof(uint32_t));
					in.read((char*)data.data(), size);
				}
				else
				{
					EK_CORE_TRACE("Compiling shader {0} to OpenGL binaries", Handle);

					spirv_cross::CompilerGLSL glslCompiler(spirv);
					spirv_cross::CompilerGLSL::Options glslOptions;
					glslOptions.vulkan_semantics = false;
					glslCompiler.set_common_options(glslOptions);

					uint32_t locationCounter = m_reflections[stage].maxLocation + 1;
					spirv_cross::ShaderResources resources = glslCompiler.get_shader_resources();
					for (auto& pushConstant : resources.push_constant_buffers)
					{
						glslCompiler.set_decoration(pushConstant.id, spv::DecorationLocation, locationCounter++);
					}

					openGLSourceCode[stage] = glslCompiler.compile();
					auto& source = openGLSourceCode[stage];

					EK_CORE_TRACE("OpenGL shader '{0}' - stage={1} source code:\n{2}", Name, ShaderStageToString(stage), source);

					shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, (shaderc_shader_kind)ShaderStageToShaderC(stage), shaderPath.string().c_str(), options);
					if (module.GetCompilationStatus() != shaderc_compilation_status_success)
					{
						success = false;
						EK_CORE_ERROR("Failed to compile shader {0}. {1}", Handle, module.GetErrorMessage());
					}
					else
					{
						shaderData[stage] = Vec<uint32_t>(module.cbegin(), module.cend());

						std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
						if (out.is_open())
						{
							auto& data = shaderData[stage];
							out.write((char*)data.data(), data.size() * sizeof(uint32_t));
							out.flush();
							out.close();
						}
						else
						{
							success = false;
							EK_CORE_ERROR("Failed to write OpenGL shader cache binaries to path: '{0}'", cachedPath.string());
						}
					}
				}
			}
			EK_CORE_DBG("Compiled or got binaries for OpenGL shader '{0}'", Name);
			m_isValid = success;
			return success;
		}
		void GLShader::CreateProgram()
		{
			EK_CORE_PROFILE();
			EK_CORE_TRACE("Creating OpenGL shader program '{0}'", Name);

			GLuint program = glCreateProgram();

			Vec<GLuint> shaderIDs;
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

				Vec<GLchar> infoLog(maxLength);
				glGetProgramInfoLog(program, maxLength, &maxLength, infoLog.data());
				if (infoLog.data())
					EK_CORE_ERROR("Shader linking failed ({0}): {1}", Handle, infoLog.data());
				else
					EK_CORE_ERROR("Shader linking failed ({0}): {1}", Handle, "No info log available");

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

			glValidateProgram(program);
			GLint isValid;
			glGetProgramiv(program, GL_VALIDATE_STATUS, &isValid);
			EK_ASSERT(isValid, "Shader validation failed ({0})", Handle);

			m_id = program;

			EK_CORE_DBG("Created OpenGL shader program '{0}' with id {1}", Name, m_id);
		}
	}
}