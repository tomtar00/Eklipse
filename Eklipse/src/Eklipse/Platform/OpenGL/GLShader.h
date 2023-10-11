#pragma once
#include <glad/glad.h>
#include <Eklipse/Renderer/Shader.h>

namespace Eklipse
{
	namespace OpenGL
	{
		class GLShader : public Eklipse::Shader
		{
		public:
			GLShader(const std::string& filePath);

			virtual void Bind() const override;
			virtual void Unbind() const override;
			virtual void Dispose() const override;

		private:
			void CompileOrGetVulkanBinaries(const std::unordered_map<ShaderStage, std::string>& shaderSources);
			void CompileOrGetOpenGLBinaries();
			void CreateProgram();

		private:
			uint32_t m_id;
			std::string m_filePath;

			std::unordered_map<ShaderStage, std::vector<uint32_t>> m_vulkanSPIRV;
			std::unordered_map<ShaderStage, std::vector<uint32_t>> m_openGLSPIRV;

			std::unordered_map<ShaderStage, std::string> m_openGLSourceCode;
		};
	}
}