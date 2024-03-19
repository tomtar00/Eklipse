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

    GraphicsAPI::Type GraphicsAPI::StringToType(const String& type)
    {
        if (type == "Vulkan") return GraphicsAPI::Type::Vulkan;
        if (type == "OpenGL") return GraphicsAPI::Type::OpenGL;

        EK_ASSERT(false, "Unknown API type: {0}", type);
        return GraphicsAPI::Type::Vulkan;
    }

    Unique<GraphicsAPI> GraphicsAPI::Create(GraphicsAPI::Type apiType)
    {
        EK_CORE_PROFILE();
        switch (apiType)
        {
            case GraphicsAPI::Type::Vulkan: return CreateUnique<Vulkan::VulkanAPI>();
            case GraphicsAPI::Type::OpenGL: return CreateUnique<OpenGL::OpenGLAPI>();
        }
        EK_ASSERT(false, "API {0} not implemented!", (int)apiType);
        return nullptr;
    }
}