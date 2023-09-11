#pragma once
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
			uint32_t m_id;
		};
	}
}