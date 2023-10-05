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
			GLShader(const std::string& name, const std::string& vertPath, const std::string& fragPath);

			virtual void Bind() const override;
			virtual void Unbind() const override;
			virtual void Dispose() const override;

			virtual void UploadMat4(const std::string& name, const glm::mat4& matrix) override;
			virtual void UploadInt(const std::string& name, const int value) override;

		private:
			std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);

			void CompileOrGetVulkanBinaries(const std::unordered_map<GLenum, std::string>& shaderSources);
			void CompileOrGetOpenGLBinaries();
			void CreateProgram();
			void Reflect(GLenum stage, const std::vector<uint32_t>& shaderData);

		private:
			uint32_t m_id;
			std::string m_filePath;
			std::string m_name;

			std::unordered_map<GLenum, std::vector<uint32_t>> m_vulkanSPIRV;
			std::unordered_map<GLenum, std::vector<uint32_t>> m_openGLSPIRV;

			std::unordered_map<GLenum, std::string> m_openGLSourceCode;
		};
	}
}