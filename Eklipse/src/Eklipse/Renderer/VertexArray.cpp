#include "precompiled.h"
#include "VertexArray.h"

#include <Eklipse/Renderer/Renderer.h>
#include <Eklipse/Platform/Vulkan/VKVertexArray.h>
#include <Eklipse/Platform/OpenGL/GLVertexArray.h>

namespace Eklipse
{
    Ref<VertexArray> Eklipse::VertexArray::Create()
    {
        EK_CORE_PROFILE();
        auto apiType = Renderer::GetGraphicsAPIType();
        switch (apiType)
        {
            case GraphicsAPI::Type::Vulkan: return CreateRef<Vulkan::VKVertexArray>();
            case GraphicsAPI::Type::OpenGL: return CreateRef<OpenGL::GLVertexArray>();
        }
        EK_ASSERT(false, "API {0} not implemented for Vertex Array creation", int(apiType));
        return nullptr;
    }
}