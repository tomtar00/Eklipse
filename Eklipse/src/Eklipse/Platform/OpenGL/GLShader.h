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
			GLShader(const Path& filePath);

			virtual void Bind() const override;
			virtual void Unbind() const override;
			virtual void Dispose() const override;

			inline uint32_t GetID() const { return m_id; }

		protected:
			virtual const std::string GetCacheDirectoryPath() override { return "Assets/Cache/Shader/OpenGL"; }

		private:
			void CompileOrGetOpenGLBinaries();
			void CreateProgram();

		private:
			uint32_t m_id;
	
			std::unordered_map<ShaderStage, std::vector<uint32_t>> m_openGLSPIRV;
			std::unordered_map<ShaderStage, std::string> m_openGLSourceCode;
		};
	}
}