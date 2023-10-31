#include "precompiled.h"
#include "Texture.h"

#include <Eklipse/Renderer/Renderer.h>
#include <Eklipse/Platform/Vulkan/VKTexture.h>
#include <Eklipse/Platform/OpenGL/GLTexture.h>

namespace Eklipse
{
    Ref<Texture2D> Texture2D::Create(const TextureInfo& textureInfo, const std::string& path)
    {
        auto apiType = Renderer::GetAPI();
        switch (apiType)
        {
            case ApiType::Vulkan: return CreateRef<Vulkan::VKTexture2D>(textureInfo, path);
            case ApiType::OpenGL: return CreateRef<OpenGL::GLTexture2D>(textureInfo, path);
        }
        EK_ASSERT(false, "API {0} not implemented for Texture creation", int(apiType));
        return nullptr;
    }
}
