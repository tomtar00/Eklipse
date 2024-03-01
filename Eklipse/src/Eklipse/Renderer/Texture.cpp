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
        EK_CORE_PROFILE();
        if (format == ImageFormat::R8)          return 1;
        else if (format == ImageFormat::RGB8)   return 3;
        else if (format == ImageFormat::RGBA8)  return 4;
        else                                    return 0;
    }
    ImageFormat ChannelsToFormat(int channels)
    {
        EK_CORE_PROFILE();
        if (channels == 1)      return ImageFormat::R8;
        else if (channels == 3) return ImageFormat::RGB8;
        else if (channels == 4) return ImageFormat::RGBA8;
        else 				    return ImageFormat::FORMAT_UNDEFINED;
    }
    bool LoadTextureFromFile(const Path& path, TextureData& outData)
    {
        EK_CORE_PROFILE();
        String pathStr = path.string();
        EK_CORE_TRACE("Loading texture from location: {0}", pathStr);

        int width, height/*, channels*/;
        uint8_t* data = stbi_load(pathStr.c_str(), &width, &height, /*&channels*/nullptr, STBI_rgb_alpha);

        if (data == nullptr)
        {
            EK_CORE_ERROR("Failed to load texture from location: {0}. {1}", pathStr.c_str(), stbi_failure_reason());
            return false;
        }

        ImageFormat format = ImageFormat::RGBA8;/*ChannelsToFormat(channels);*/

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
            uint32_t(width * height * 4/*channels*/)
        };

        EK_CORE_DBG("Loaded texture from path '{0}'. Width: {1} Height: {2} Channels: {3}", pathStr, width, height, 4/*channels*/);
        return true;
    }

    Texture2D::Texture2D(const Path& path, const AssetHandle handle)
    {      
        Handle = handle;
    }
    Texture2D::Texture2D(const TextureInfo& textureInfo)
    { 
    }
    Texture2D::Texture2D(const TextureData& textureData)
    {   
    }

    Ref<Texture2D> Texture2D::Create(const Path& path, const AssetHandle handle)
    {
        EK_CORE_PROFILE();
        switch (Renderer::GetAPI())
        {
            case GraphicsAPI::Type::Vulkan: return CreateRef<Vulkan::VKTexture2D>(path, handle);
            case GraphicsAPI::Type::OpenGL: return CreateRef<OpenGL::GLTexture2D>(path, handle);
        }
        EK_ASSERT(false, "Texture creation not implemented for current graphics API");
        return nullptr;
    }
    Ref<Texture2D> Texture2D::Create(const TextureInfo& textureInfo)
    {
        EK_CORE_PROFILE();
        switch (Renderer::GetAPI())
        {
            case GraphicsAPI::Type::Vulkan: return CreateRef<Vulkan::VKTexture2D>(textureInfo);
            case GraphicsAPI::Type::OpenGL: return CreateRef<OpenGL::GLTexture2D>(textureInfo);
        }
        EK_ASSERT(false, "Texture creation not implemented for current graphics API");
        return nullptr;
    }
    Ref<Texture2D> Texture2D::Create(const TextureData& textureData)
    {
        EK_CORE_PROFILE();
        switch (Renderer::GetAPI())
        {
            case GraphicsAPI::Type::Vulkan: return CreateRef<Vulkan::VKTexture2D>(textureData);
            case GraphicsAPI::Type::OpenGL: return CreateRef<OpenGL::GLTexture2D>(textureData);
        }
        EK_ASSERT(false, " Texture creation not implemented for current graphics API");
        return nullptr;
    }
    
    const TextureInfo& Texture2D::GetInfo() const
    {
        return m_textureInfo;
    }
}