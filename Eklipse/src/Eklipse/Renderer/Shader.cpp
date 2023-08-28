#include "precompiled.h"
#include "Shader.h"

#include <Eklipse/Renderer/Renderer.h>
#include <Eklipse/Platform/OpenGL/GLShader.h>
#include <Eklipse/Platform/Vulkan/VKShader.h>

namespace Eklipse
{
    Ref<Shader> Shader::Create(const std::string& name, const std::string& vertPath, const std::string& fragPath)
    {
        auto apiType = Renderer::GetAPI();
        switch (apiType)
        {
            case ApiType::Vulkan: return CreateRef<Vulkan::VKShader>(name, vertPath, fragPath);
            case ApiType::OpenGL: return CreateRef<OpenGL::GLShader>(name, vertPath, fragPath);
        }
        EK_ASSERT(false, "API {0} not implemented for Shader creation", int(apiType));
        return nullptr;
    }
    const std::string& Shader::GetName() const
    {
        return m_name;
    }

    //////////////////////////////////////////////////////////////////
    // SHADER LIBRARY ////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////

    void ShaderLibrary::Add(const Ref<Shader>& shader)
    {
        if (Contains(shader->GetName()))
        {
            EK_CORE_WARN("Shader library already contains shader '{0}'", shader->GetName());
            return;
        }

        m_shaders[shader->GetName()] = shader;
        EK_CORE_INFO("Adding shader '{0}' to library", shader->GetName());
    }
    Ref<Shader> ShaderLibrary::Load(const std::string& name, const std::string& vertPath, const std::string& fragPath)
    {
        auto shader = Shader::Create(name, vertPath, fragPath);
        Add(shader);
        return shader;
    }
    Ref<Shader> ShaderLibrary::Get(const std::string& name)
    {
        EK_ASSERT(Contains(name), "Shader '{0}' not present in library", name);
        return m_shaders[name];
    }
    bool ShaderLibrary::Contains(const std::string& name)
    {
        return m_shaders.find(name) != m_shaders.end();
    }
    void ShaderLibrary::Dispose()
    {
        for (auto& shader : m_shaders)
            shader.second->Dispose();
    }
}