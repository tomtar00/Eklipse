#include "precompiled.h"
#include "Buffers.h"

#include <Eklipse/Renderer/Renderer.h>
#include <Eklipse/Platform/Vulkan/VKBuffers.h>
#include <Eklipse/Platform/OpenGL/GLBuffers.h>

namespace Eklipse
{
    BufferLayout::BufferLayout(std::initializer_list<BufferElement> elements) : m_elements(elements)
    {
        EK_CORE_PROFILE();
        CalculateOffsetAndStride();
    }

    void BufferLayout::CalculateOffsetAndStride()
    {
        EK_CORE_PROFILE();
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
        EK_CORE_PROFILE();
        switch (type)
        {
            case ShaderDataType::FLOAT:   return 1;
            case ShaderDataType::FLOAT2:  return 2;
            case ShaderDataType::FLOAT3:  return 3;
            case ShaderDataType::FLOAT4:  return 4;
            case ShaderDataType::MAT3:    return 9;
            case ShaderDataType::MAT4:    return 16;
            case ShaderDataType::INT:     return 1;
            case ShaderDataType::INT2:    return 2;
            case ShaderDataType::INT3:    return 3;
            case ShaderDataType::INT4:    return 4;
            case ShaderDataType::BOOL:    return 1;
        }

        EK_ASSERT(false, "Unknown ShaderDataType!");
        return 0;
    }

    Ref<VertexBuffer> VertexBuffer::Create(const Vec<float>& vertices)
    {
        EK_CORE_PROFILE();
        auto apiType = Renderer::GetGraphicsAPIType();
        switch (apiType)
        {
            case GraphicsAPI::Type::Vulkan: return CreateRef<Vulkan::VKVertexBuffer>(vertices);
            case GraphicsAPI::Type::OpenGL: return CreateRef<OpenGL::GLVertexBuffer>(vertices);
        }
        EK_ASSERT(false, "API {0} not implemented for Vertex Buffer creation", int(apiType));
        return nullptr;
    }
    Ref<IndexBuffer> IndexBuffer::Create(const Vec<uint32_t>& indices)
    {
        EK_CORE_PROFILE();
        auto apiType = Renderer::GetGraphicsAPIType();
        switch (apiType)
        {
            case GraphicsAPI::Type::Vulkan: return CreateRef<Vulkan::VKIndexBuffer>(indices);
            case GraphicsAPI::Type::OpenGL: return CreateRef<OpenGL::GLIndexBuffer>(indices);
        }
        EK_ASSERT(false, "API {0} not implemented for Index Buffer creation", int(apiType));
        return nullptr;
    }
    Ref<UniformBuffer> UniformBuffer::Create(size_t size, uint32_t binding)
    {
        EK_CORE_PROFILE();
        auto apiType = Renderer::GetGraphicsAPIType();
        switch (apiType)
        {
            case GraphicsAPI::Type::Vulkan: return CreateRef<Vulkan::VKUniformBuffer>(size, binding);
            case GraphicsAPI::Type::OpenGL: return CreateRef<OpenGL::GLUniformBuffer>(size, binding);
        }
        EK_ASSERT(false, "API {0} not implemented for Uniform Buffer creation", int(apiType));
        return nullptr;
    }
    Ref<StorageBuffer> StorageBuffer::Create(size_t size, uint32_t binding)
    {
        EK_CORE_PROFILE();
        auto apiType = Renderer::GetGraphicsAPIType();
        switch (apiType)
        {
            case GraphicsAPI::Type::Vulkan: return CreateRef<Vulkan::VKStorageBuffer>(size, binding);
            case GraphicsAPI::Type::OpenGL: return CreateRef<OpenGL::GLStorageBuffer>(size, binding);
        }
        EK_ASSERT(false, "API {0} not implemented for Storage Buffer creation", int(apiType));
        return nullptr;
    }
}