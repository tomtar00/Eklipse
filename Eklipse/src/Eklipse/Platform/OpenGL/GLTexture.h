#pragma once

#include <glad/glad.h>
#include <Eklipse/Renderer/Texture.h>

namespace Eklipse
{
	namespace OpenGL
	{
		class GLTexture2D : public Texture2D
		{
		public:
			GLTexture2D() = delete;

			virtual void Init(const TextureInfo& info) override;
			virtual void SetData(void* data, uint32_t size) override;
			virtual void Bind() const override;
			virtual void Unbind() const override;
			virtual void Dispose() override;

			inline uint32_t GetID() const { return m_id; }

		private:
			uint32_t m_id;
			GLenum m_internalFormat, m_format;
		};
	}
}