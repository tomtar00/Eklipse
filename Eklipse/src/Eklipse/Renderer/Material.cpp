#include "precompiled.h"
#include "Material.h"

#include <Eklipse/Renderer/Renderer.h>
#include <Eklipse/Platform/Vulkan/VKMaterial.h>
#include <Eklipse/Platform/OpenGL/GLMaterial.h>

namespace Eklipse
{
    Ref<Material> Eklipse::Material::Create(Ref<Shader> shader)
    {
        auto apiType = Renderer::GetAPI();
        switch (apiType)
        {
            case ApiType::Vulkan: return CreateRef<Vulkan::VKMaterial>(shader);
            case ApiType::OpenGL: return CreateRef<OpenGL::GLMaterial>(shader);
        }
        EK_ASSERT(false, "API {0} not implemented for Material creation", int(apiType));
        return nullptr;
    }

    void Material::Bind()
    {
        m_shader->Bind();
    }
    void Material::Dispose()
    {
        m_shader->Dispose();
    }
}