#include "precompiled.h"
#include "Texture.h"

#include <Eklipse/Renderer/Renderer.h>
#include <Eklipse/Platform/Vulkan/VKTexture.h>
#include <Eklipse/Platform/OpenGL/GLTexture.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Eklipse
{
    int FormatToChannels(ImageFormat format)
    {
        if (format == ImageFormat::R8)          return 1;
        else if (format == ImageFormat::RGB8)   return 3;
        else if (format == ImageFormat::RGBA8)  return 4;
        else                                    return 0;
    }
    ImageFormat ChannelsToFormat(int channels)
    {
        if (channels == 1)      return ImageFormat::R8;
		else if (channels == 3) return ImageFormat::RGB8;
		else if (channels == 4) return ImageFormat::RGBA8;
        else 				    return ImageFormat::FORMAT_UNDEFINED;
    }
    static bool LoadTextureFromFile(const Path& path, TextureData& outData)
    {
        const char* pathStr = path.string().c_str();
        EK_CORE_TRACE("Loading texture from path: {0}", pathStr);

        int width, height, channels;
        uint8_t* data = stbi_load(pathStr, &width, &height, &channels, STBI_rgb_alpha);

        if (data == nullptr)
        {
            EK_CORE_ERROR("Failed to load texture from location: {0}", pathStr);
            return false;
        }

        ImageFormat format = ChannelsToFormat(channels);

        TextureInfo textureInfo{};
        textureInfo.width = width;
        textureInfo.height = height;
        textureInfo.mipMapLevel = 1;
        textureInfo.samples = 1;
        textureInfo.imageFormat = format;
        textureInfo.imageAspect = ImageAspect::COLOR;
        textureInfo.imageUsage = ImageUsage::SAMPLED | ImageUsage::TRASNFER_DST;

        outData = TextureData
        {
            textureInfo,
			data,
			uint32_t(width * height * channels)
        };

        EK_CORE_DBG("Loaded texture from path '{0}'. Width: {1} Height: {2} Channels: {3}", pathStr, width, height, channels);
        return true;
	}

    Texture2D::Texture2D(const Path& path)
    {
        TextureData textureData{};
        if (LoadTextureFromFile(path, textureData))
        {
            m_textureInfo = textureData.info;
            Init(m_textureInfo);
            SetData(textureData.data, textureData.size);
        }
    }
    Texture2D::Texture2D(const TextureInfo& textureInfo)
    {
        m_textureInfo = textureInfo;
        Init(m_textureInfo);
    }
    Texture2D::Texture2D(const TextureData& textureData)
    {
        m_textureInfo = textureData.info;
        Init(m_textureInfo);
		SetData(textureData.data, textureData.size);
    }

    Ref<Texture2D> Texture2D::Create(const Path& path)
    {
        switch (Renderer::GetAPI())
        {
            case ApiType::Vulkan: return CreateRef<Vulkan::VKTexture2D>(path);
            case ApiType::OpenGL: return CreateRef<OpenGL::GLTexture2D>(path);
        }
        EK_ASSERT(false, "API {0} not implemented for current graphics API Texture creation");
        return nullptr;
    }
    Ref<Texture2D> Texture2D::Create(const TextureInfo& textureInfo)
    {
        switch (Renderer::GetAPI())
        {
            case ApiType::Vulkan: return CreateRef<Vulkan::VKTexture2D>(textureInfo);
            case ApiType::OpenGL: return CreateRef<OpenGL::GLTexture2D>(textureInfo);
        }
        EK_ASSERT(false, "API {0} not implemented for current graphics Texture creation");
        return nullptr;
    }
    Ref<Texture2D> Texture2D::Create(const TextureData& textureData)
    {
        switch (Renderer::GetAPI())
        {
            case ApiType::Vulkan: return CreateRef<Vulkan::VKTexture2D>(textureData);
            case ApiType::OpenGL: return CreateRef<OpenGL::GLTexture2D>(textureData);
        }
        EK_ASSERT(false, "API {0} not implemented for current graphics Texture creation");
        return nullptr;
    }
}
