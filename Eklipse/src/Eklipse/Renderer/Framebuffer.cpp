#include "precompiled.h"
#include "Framebuffer.h"

#include <Eklipse/Renderer/Renderer.h>
#include <Eklipse/Platform/Vulkan/VKFramebuffer.h>
#include <Eklipse/Platform/OpenGL/GLFramebuffer.h>

namespace Eklipse
{
    Framebuffer* g_defaultFramebuffer = nullptr;
    Framebuffer* g_currentFramebuffer = nullptr;
    Vec<Framebuffer*> g_offScreenFramebuffers{};

    Framebuffer::Framebuffer(const FramebufferInfo& framebufferInfo)
    {
        m_aspectRatio = (float)framebufferInfo.width / (float)framebufferInfo.height;
    }

    Ref<Framebuffer> Eklipse::Framebuffer::Create(const FramebufferInfo& framebufferInfo)
    {
        EK_CORE_PROFILE();
        switch (Renderer::GetGraphicsAPIType())
        {
            case GraphicsAPI::Type::Vulkan: return CreateRef<Vulkan::VKFramebuffer>(framebufferInfo);
            case GraphicsAPI::Type::OpenGL: return CreateRef<OpenGL::GLFramebuffer>(framebufferInfo);
        }
        EK_ASSERT(false, "Framebuffer creation not implemented for current graphics API");
        return nullptr;
    }

    void Framebuffer::Resize(uint32_t width, uint32_t height)
    {
        EK_CORE_PROFILE();
        m_aspectRatio = (float)width / (float)height;
    }
    const float Framebuffer::GetAspectRatio() const
    {
        return m_aspectRatio;
    }
}