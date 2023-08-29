#include "precompiled.h"
#include "GLTexture.h"

#include <stb_image.h>

namespace Eklipse
{
	namespace OpenGL
	{
		static GLenum ConvertToGLFormat(ImageFormat format)
		{
			switch (format)
			{
				case ImageFormat::RGB8:  return GL_RGB;
				case ImageFormat::RGBA8: return GL_RGBA;
			}

			EK_ASSERT(false, "Wrong image format");
			return 0;
		}

		static GLenum ConvertToInternalGLFormat(ImageFormat format)
		{
			switch (format)
			{
				case ImageFormat::RGB8:  return GL_RGB8;
				case ImageFormat::RGBA8: return GL_RGBA8;
			}

			EK_ASSERT(false, "Wrong internal image format");
			return 0;
		}

		GLTexture2D::GLTexture2D(const std::string& texturePath) : m_id(0)
		{
			int width, height, channels;
			stbi_uc* data = stbi_load(texturePath.c_str(), &width, &height, &channels, 0);
			EK_ASSERT(data, "Failed to load texture from path '{0}'", texturePath);

			m_textureInfo.width = width;
			m_textureInfo.height = height;

			GLenum format = 0, dataFormat = 0;
			if (channels == 4)
			{
				format = GL_RGBA8;
				dataFormat = GL_RGBA;
			}
			else if (channels == 3)
			{
				format = GL_RGB8;
				dataFormat = GL_RGB;
			}

			m_internalFormat = format;
			m_format = dataFormat;

			EK_ASSERT(format & dataFormat, "Texture format not supported!");

			glGenTextures(1, &m_id);
			glBindTexture(GL_TEXTURE_2D, m_id);

			glTexParameteri(m_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(m_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(m_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(m_id, GL_TEXTURE_WRAP_T, GL_REPEAT);

			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);

			stbi_image_free(data);
		}
		GLTexture2D::GLTexture2D(const TextureInfo& textureInfo) : m_id(0)
		{
			m_format = ConvertToGLFormat(textureInfo.imageFormat);
			m_internalFormat = ConvertToInternalGLFormat(textureInfo.imageFormat);

			glGenTextures(1, &m_id);
			glBindTexture(GL_TEXTURE_2D, m_id);

			glTexParameteri(m_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(m_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(m_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(m_id, GL_TEXTURE_WRAP_T, GL_REPEAT);
		}
		GLTexture2D::~GLTexture2D()
		{
			glDeleteTextures(1, &m_id);
		}
		void GLTexture2D::SetData(void* data, uint32_t size)
		{
			uint32_t singlePixelSize = m_format == GL_RGBA ? 4 : 3;
			uint32_t dataSize = m_textureInfo.width * m_textureInfo.height * singlePixelSize;
			EK_ASSERT(size == dataSize, "Data is smaller than the required size of the texture!");
			glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, m_textureInfo.width, m_textureInfo.height, 0, m_format, GL_UNSIGNED_BYTE, data);
		}
		void GLTexture2D::Bind() const
		{
			glBindTexture(GL_TEXTURE_2D, m_id);
		}
	}
}
