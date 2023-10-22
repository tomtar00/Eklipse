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

    Material::Material(Ref<Shader> shader) : m_shader(shader)
    {
        for (auto&& [stage, reflection] : shader->GetReflections())
        {
            for (auto& pushConstantRef : reflection.pushConstants)
            {
                auto& pushConstant = m_pushConstants[pushConstantRef.name];
                pushConstant.pushConstantData = std::make_unique<char[]>(pushConstantRef.size);
                pushConstant.pushConstantSize = pushConstantRef.size;

                uint32_t offset = 0;
                for (auto& member : pushConstantRef.members)
                {
                    pushConstant.dataPointers[member.name] = { pushConstant.pushConstantData.get() + offset, member.size};
                    offset += member.size;
                }
			}
        }
    }

    void Material::SetConstant(const std::string& constantName, const std::string& memberName, const void* data, size_t size)
    {
        EK_ASSERT(m_pushConstants.find(constantName) != m_pushConstants.end(), "Push constant '{0}' not found", constantName);
        auto& pushConstant = m_pushConstants[constantName];
        EK_ASSERT(pushConstant.dataPointers.find(memberName) != pushConstant.dataPointers.end(), "Push constant '{0}' member '{1}' not found", constantName, memberName);

        auto& dataPointer = pushConstant.dataPointers[memberName];
        EK_ASSERT(dataPointer.size == size, "Push constant '{0}' member '{1}' size mismatch", constantName, memberName);
        memcpy(dataPointer.data, data, size);
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