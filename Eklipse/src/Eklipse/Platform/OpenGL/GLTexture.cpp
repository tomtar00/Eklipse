#include "precompiled.h"
#include "GLTexture.h"

#include <stb_image.h>

namespace Eklipse
{
    namespace OpenGL
    {
        static GLenum ConvertToGLFormat(ImageFormat format)
        {
            EK_CORE_PROFILE();
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
            EK_CORE_PROFILE();
            switch (format)
            {
                case ImageFormat::RGB8:  return GL_RGB8;
                case ImageFormat::RGBA8: return GL_RGBA8;
            }
            EK_ASSERT(false, "Wrong internal image format");
            return 0;
        }

        GLTexture2D::GLTexture2D(const Path& path, const AssetHandle handle) : Texture2D(path, handle)
        {
            EK_CORE_PROFILE();
            TextureData textureData{};
            if (LoadTextureFromFile(path, textureData))
            {
                m_textureInfo = textureData.info;
                Init(m_textureInfo);
                SetData(textureData.data, textureData.size);
            }
        }
        GLTexture2D::GLTexture2D(const TextureInfo& textureInfo) : Texture2D(textureInfo)
        {
            EK_CORE_PROFILE();
            m_textureInfo = textureInfo;
            Init(m_textureInfo);
        }
        GLTexture2D::GLTexture2D(const TextureData& textureData) : Texture2D(textureData)
        {
            EK_CORE_PROFILE();
            m_textureInfo = textureData.info;
            Init(m_textureInfo);
            SetData(textureData.data, textureData.size);
        }

        void GLTexture2D::Init(const TextureInfo& textureInfo)
        {
            EK_CORE_PROFILE();
            m_format = ConvertToGLFormat(textureInfo.imageFormat);
            m_internalFormat = ConvertToInternalGLFormat(textureInfo.imageFormat);

            m_textureInfo.width = textureInfo.width;
            m_textureInfo.height = textureInfo.height;

            glGenTextures(1, &m_id);
            glBindTexture(GL_TEXTURE_2D, m_id);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        }
        void GLTexture2D::SetData(void* data, uint32_t size)
        {
            EK_CORE_PROFILE();
            uint32_t singlePixelSize = FormatToChannels(m_textureInfo.imageFormat);
            uint32_t dataSize = m_textureInfo.width * m_textureInfo.height * singlePixelSize;
            EK_ASSERT((size == dataSize), "Data is not equal required size of the texture! Given: {0} Required: {1}", size, dataSize);
            
            if (m_textureInfo.samples > 1)
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_textureInfo.samples, m_internalFormat, m_textureInfo.width, m_textureInfo.height, GL_FALSE);
            else
            {
                if (m_textureInfo.imageFormat == ImageFormat::D24S8)
                    glTexStorage2D(GL_TEXTURE_2D, 1, m_format, m_textureInfo.width, m_textureInfo.height);
                else
                    glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, m_textureInfo.width, m_textureInfo.height, 0, m_format, GL_UNSIGNED_BYTE, data);
            }
            
            if (m_textureInfo.mipMapLevel > 1)
                glGenerateMipmap(GL_TEXTURE_2D);
        }
        void GLTexture2D::Bind() const
        {
            EK_CORE_PROFILE();
            glBindTexture(GL_TEXTURE_2D, m_id);
        }
        void GLTexture2D::Unbind() const
        {
            EK_CORE_PROFILE();
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        void GLTexture2D::Dispose()
        {
            EK_CORE_PROFILE();
            glDeleteTextures(1, &m_id);
        }
    }
}
