#include "precompiled.h"
#include "Buffers.h"

#include <Eklipse/Renderer/Renderer.h>
#include <Eklipse/Platform/Vulkan/VKBuffers.h>
#include <Eklipse/Platform/OpenGL/GLBuffers.h>

namespace Eklipse
{
    Ref<VertexBuffer> VertexBuffer::Create(std::vector<Vertex> vertices)
    {
        auto apiType = Renderer::GetAPI();
        switch (apiType)
        {
            case ApiType::Vulkan: return CreateRef<Vulkan::VKVertexBuffer>(vertices);
            case ApiType::OpenGL: return CreateRef<OpenGL::GLVertexBuffer>(vertices);
        }
        EK_ASSERT(false, "API {0} not implemented for Vertex Buffer creation", int(apiType));
        return nullptr;
    }

    Ref<IndexBuffer> IndexBuffer::Create(std::vector<uint32_t> indices)
    {
        auto apiType = Renderer::GetAPI();
        switch (apiType)
        {
            case ApiType::Vulkan: return CreateRef<Vulkan::VKIndexBuffer>(indices);
            case ApiType::OpenGL: return CreateRef<OpenGL::GLIndexBuffer>(indices);
        }
        EK_ASSERT(false, "API {0} not implemented for Index Buffer creation", int(apiType));
        return nullptr;
    }

    Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
    {
        auto apiType = Renderer::GetAPI();
        switch (apiType)
        {
            case ApiType::Vulkan: return CreateRef<Vulkan::VKUniformBuffer>(size, binding);
            case ApiType::OpenGL: return CreateRef<OpenGL::GLUniformBuffer>(size, binding);
        }
        EK_ASSERT(false, "API {0} not implemented for Uniform Buffer creation", int(apiType));
        return nullptr;
    }
}