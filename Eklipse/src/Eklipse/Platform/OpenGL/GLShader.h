#pragma once
#include <glad/glad.h>
#include <Eklipse/Renderer/Shader.h>

namespace Eklipse
{
	namespace OpenGL
	{
		class GLShader : public Shader
		{
		public:
			GLShader(const Path& filePath);

			uint32_t GetID() const;

			virtual void Bind() const override;
			virtual void Unbind() const override;
			virtual void Dispose() const override;

		protected:
			virtual const std::string GetCacheDirectoryPath() override;
			virtual bool Compile(bool forceCompile = false) override;

		private:
			bool CompileOrGetOpenGLBinaries(bool forceCompile);
			void CreateProgram();

		private:
			uint32_t m_id;
	
			StageSpirvMap m_openGLSPIRV;
			StageSourceMap m_openGLSourceCode;
		};
	}
}