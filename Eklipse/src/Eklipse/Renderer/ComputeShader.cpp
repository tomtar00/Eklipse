#include "precompiled.h"
#include "ComputeShader.h"

#include <Eklipse/Renderer/Renderer.h>
#include <Eklipse/Platform/OpenGL/GLComputeShader.h>
#include <Eklipse/Platform/Vulkan/VKComputeShader.h>

namespace Eklipse
{
    ComputeShader::ComputeShader(const String& filePath)
    {
        m_shader = Shader::Create(filePath);
        m_material = Material::Create(m_shader);
    }
    Ref<ComputeShader> ComputeShader::Create(const String& filePath)
    {
        EK_CORE_PROFILE();
        switch (Renderer::GetGraphicsAPIType())
        {
            case GraphicsAPI::Type::Vulkan: return CreateRef<Vulkan::VKComputeShader>(filePath);
            case GraphicsAPI::Type::OpenGL: return CreateRef<OpenGL::GLComputeShader>(filePath);
        }
        EK_ASSERT(false, "Shader creation not implemented for current graphics API");
        return nullptr;
    }
    Ref<Shader> ComputeShader::GetShader() const
    {
        return m_shader;
    }
    Ref<Material> ComputeShader::GetMaterial() const
    {
        return m_material;
    }
    void ComputeShader::Dispose()
    {
        m_shader->Dispose();
        m_material->Dispose();
    }
}