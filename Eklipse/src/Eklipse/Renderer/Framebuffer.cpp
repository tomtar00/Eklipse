#include "precompiled.h"
#include "Framebuffer.h"

#include <Eklipse/Renderer/Renderer.h>
#include <Eklipse/Platform/Vulkan/VKFramebuffer.h>
#include <Eklipse/Platform/OpenGL/GLFramebuffer.h>

namespace Eklipse
{
    Framebuffer* g_defaultFramebuffer   = nullptr;
    Framebuffer* g_sceneFramebuffer     = nullptr;
    Framebuffer* g_currentFramebuffer   = nullptr;

    Ref<Framebuffer> Eklipse::Framebuffer::Create(const FramebufferInfo& framebufferInfo)
    {
        auto apiType = Renderer::GetAPI();
        switch (apiType)
        {
            case ApiType::Vulkan: return CreateRef<Vulkan::VKFramebuffer>(framebufferInfo);
            case ApiType::OpenGL: return CreateRef<OpenGL::GLFramebuffer>(framebufferInfo);
        }
        EK_ASSERT(false, "API {0} not implemented for Framebuffer creation", int(apiType));
        return nullptr;
    }

    void Framebuffer::Resize(uint32_t width, uint32_t height)
    {
        m_aspectRatio = (float)width / (float)height;
    }
}
