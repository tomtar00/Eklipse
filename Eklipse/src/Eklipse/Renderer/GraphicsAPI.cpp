#include "precompiled.h"
#include "GraphicsAPI.h"

#include <Eklipse/Renderer/Renderer.h>
#include <Eklipse/Platform/Vulkan/VulkanAPI.h>
#include <Eklipse/Platform/OpenGL/OpenGLAPI.h>

namespace Eklipse
{
    GraphicsAPI::GraphicsAPI() : m_initialized(false) {}

    const String GraphicsAPI::TypeToString(Type type)
    {
        switch (type)
        {
            case GraphicsAPI::Type::Vulkan: return "Vulkan";
            case GraphicsAPI::Type::OpenGL: return "OpenGL";
        }
        return "Unknown";
    }

    Unique<GraphicsAPI> GraphicsAPI::Create()
    {
        EK_CORE_PROFILE();
        auto apiType = Renderer::GetGraphicsAPIType();
        switch (apiType)
        {
            case GraphicsAPI::Type::Vulkan: return CreateUnique<Vulkan::VulkanAPI>();
            case GraphicsAPI::Type::OpenGL: return CreateUnique<OpenGL::OpenGLAPI>();
        }
        EK_ASSERT(false, "API {0} not implemented!", (int)apiType);
        return nullptr;
    }
}