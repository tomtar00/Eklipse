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
			GLShader(const Path& filePath, const AssetHandle handle = AssetHandle());

			uint32_t GetID() const;

			virtual void Bind() const override;
			virtual void Unbind() const override;
			virtual void Dispose() const override;

		protected:
			virtual const String GetCacheDirectoryPath() override;
			virtual bool Compile(const Path& shaderPath, bool forceCompile = false) override;

		private:
			bool CompileOrGetOpenGLBinaries(const Path& shaderPath, bool forceCompile);
			void CreateProgram();

		private:
			uint32_t m_id;
	
			StageSpirvMap m_openGLSPIRV;
			StageSourceMap m_openGLSourceCode;
		};
	}
}