#include "precompiled.h"
#include "Buffers.h"

#include <Eklipse/Renderer/Renderer.h>
#include <Eklipse/Platform/Vulkan/VKBuffers.h>
#include <Eklipse/Platform/OpenGL/GLBuffers.h>

namespace Eklipse
{
    BufferLayout::BufferLayout(std::initializer_list<BufferElement> elements) : m_elements(elements)
    {
        CalculateOffsetAndStride();
    }

    void BufferLayout::CalculateOffsetAndStride()
    {
        size_t offset = 0;
        m_stride = 0;
        for (auto& element : m_elements)
        {
            element.offset = offset;
            offset += element.size;
            m_stride += element.size;
        }
    }
    uint32_t BufferElement::GetComponentCount() const
    {
        switch (type)
        {
        case ShaderDataType::Float:   return 1;
        case ShaderDataType::Float2:  return 2;
        case ShaderDataType::Float3:  return 3;
        case ShaderDataType::Float4:  return 4;
        case ShaderDataType::Mat3:    return 3;
        case ShaderDataType::Mat4:    return 4;
        case ShaderDataType::Int:     return 1;
        case ShaderDataType::Int2:    return 2;
        case ShaderDataType::Int3:    return 3;
        case ShaderDataType::Int4:    return 4;
        case ShaderDataType::Bool:    return 1;
        }

        EK_ASSERT(false, "Unknown ShaderDataType!");
        return 0;
    }

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