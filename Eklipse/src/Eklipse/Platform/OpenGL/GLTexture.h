#pragma once

#include <glad/glad.h>
#include <Eklipse/Renderer/Texture.h>

namespace Eklipse
{
	namespace OpenGL
	{
		class GLTexture2D : public Eklipse::Texture2D
		{
		public:
			GLTexture2D(const std::string& texturePath);
			GLTexture2D(const TextureInfo& textureInfo);
			virtual ~GLTexture2D();

			virtual void SetData(void* data, uint32_t size) override;
			virtual void Bind() const override;

		private:
			uint32_t m_id;
			GLenum m_internalFormat, m_format;
		};
	}
}