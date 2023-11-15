#include "precompiled.h"
#include "Material.h"

#include <Eklipse/Renderer/Renderer.h>
#include <Eklipse/Platform/Vulkan/VKMaterial.h>
#include <Eklipse/Platform/OpenGL/GLMaterial.h>
#include <Eklipse/Core/Application.h>
#include <Eklipse/Utils/Yaml.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

namespace Eklipse
{
    static std::string DataTypeToString(DataType type)
    {
        switch (type)
        {
			case DataType::FLOAT:   return "float";
			case DataType::FLOAT2:  return "float2";
			case DataType::FLOAT3:  return "float3";
			case DataType::FLOAT4:  return "float4";
			case DataType::MAT3:    return "mat3";
			case DataType::MAT4:    return "mat4";
			case DataType::INT:     return "int";
			case DataType::INT2:    return "int2";
			case DataType::INT3:    return "int3";
			case DataType::INT4:    return "int4";
			case DataType::BOOL:    return "bool";
		}
		EK_ASSERT(false, "Unknown data type");
		return "UNKNOWN";
	}
    template<typename T>
    static void SetData(void* dst, YAML::Node& node)
    {
        if (node.IsNull()) 
        {
            EK_CORE_WARN("Failed to deserialize data, node '{0}' is null", node.Tag());
            dst = nullptr;
            return;
        }

        T data = node.as<T>();
        dst = &data;
    }

    Ref<Material> Eklipse::Material::Create(const Path& path, const Path& shaderPath)
    {
        auto apiType = Renderer::GetAPI();
        switch (apiType)
        {
            case ApiType::Vulkan: return CreateRef<Vulkan::VKMaterial>(path, shaderPath);
            case ApiType::OpenGL: return CreateRef<OpenGL::GLMaterial>(path, shaderPath);
        }
        EK_ASSERT(false, "API {0} not implemented for Material creation", int(apiType));
        return nullptr;
    }

    Material::Material(const Path& path, const Path& shaderPath) : m_path(path)
    {
        m_name = path.path().stem().string();

        if (path.isValid(".ekmt"))
        {
            Deserialize(path);
        }
        else
        {
            SetShader(Project::GetActive()->GetAssetLibrary()->GetShader(shaderPath));
            Serialize(path);
        }
    }

    void Material::Bind()
    {
        m_shader->Bind();
    }

    void Material::ApplyChanges()
    {
        Serialize(m_path);
        Deserialize(m_path);
    }

    void Material::SetShader(Ref<Shader> shader)
    {
        m_shader = shader;
        m_pushConstants.clear();
        m_samplers.clear();

        for (auto&& [stage, reflection] : shader->GetReflections())
        {
            for (auto& pushConstantRef : reflection.pushConstants)
            {
                auto& pushConstant = m_pushConstants[pushConstantRef.name];
                pushConstant.pushConstantData = std::make_unique<char[]>(pushConstantRef.size);
                pushConstant.pushConstantSize = pushConstantRef.size;

                uint32_t offset = 0;
                pushConstant.dataPointers.clear();
                for (auto& member : pushConstantRef.members)
                {
                    pushConstant.dataPointers[member.name] = { pushConstant.pushConstantData.get() + offset, member.size, member.type };
                    offset += member.size;
                }
            }
            for (auto& samplerRef : reflection.samplers)
            {
                m_samplers[samplerRef.name] = { samplerRef.binding, "", nullptr };
            }
        }
    }

    void Material::Serialize(const Path& path)
    {
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Name" << YAML::Value << m_name;
        out << YAML::Key << "Shader" << YAML::Value << m_shader->GetPath().generic_string();

        {
            out << YAML::Key << "PushConstants" << YAML::Value << YAML::BeginMap;
            for (auto&& [constantName, pushConstant] : m_pushConstants)
            {
                out << YAML::Key << constantName << YAML::Value << YAML::BeginSeq;
                for (auto&& [memberName, data] : pushConstant.dataPointers)
                {
                    out << YAML::BeginMap;
                    out << YAML::Key << "Name" << YAML::Value << memberName;
                    out << YAML::Key << "Type" << YAML::Value << DataTypeToString(data.type);
                    out << YAML::Key << "Data" << YAML::Value;
                    switch (data.type)
                    {
                        case DataType::FLOAT:   out << *(float*)data.data;      break;
                        case DataType::FLOAT2:  out << *(glm::vec2*)data.data;  break;
                        case DataType::FLOAT3:  out << *(glm::vec3*)data.data;  break;
                        case DataType::FLOAT4:  out << *(glm::vec4*)data.data;  break;
                        case DataType::MAT3:    out << *(glm::mat3*)data.data;  break;
                        case DataType::MAT4:    out << *(glm::mat4*)data.data;  break;
                        case DataType::INT:     out << *(int*)data.data;        break;
                        case DataType::INT2:    out << *(glm::ivec2*)data.data; break;
                        case DataType::INT3:    out << *(glm::ivec3*)data.data; break;
                        case DataType::INT4:    out << *(glm::ivec4*)data.data; break;
                        case DataType::BOOL:    out << *(bool*)data.data;       break;
                    }
                    out << YAML::EndMap;
                }
                out << YAML::EndSeq;
            }
            out << YAML::EndMap;
        }

        {
            out << YAML::Key << "Samplers" << YAML::Value << YAML::BeginMap;
            for (auto&& [samplerName, sampler] : m_samplers)
            {
				out << YAML::Key << samplerName << YAML::Value << sampler.texturePath.generic_string();
			}
			out << YAML::EndMap;
        }

        out << YAML::EndMap;

        std::ofstream fout(path.string());
        fout << out.c_str();
    }
    void Material::Deserialize(const Path& path)
    {
        YAML::Node yaml;
        try
        {
            yaml = YAML::LoadFile(path.string());
        }
        catch (std::runtime_error e)
        {
            EK_CORE_ERROR("Failed to load .ekmt file '{0}'\n     {1}", path.string(), e.what());
            return;
        }

        if (!yaml["Name"])
        {
			EK_CORE_ERROR("Material file '{0}' is missing a name", path.string());
			return;
		}

        m_name = yaml["Name"].as<std::string>();
		m_path = path;

        if (!yaml["Shader"])
        {
            EK_CORE_ERROR("Material file '{0}' is missing a shader path", path.string());
            return;
        }

        m_shader = Project::GetActive()->GetAssetLibrary()->GetShader(yaml["Shader"].as<std::string>());
        SetShader(m_shader);

        for (auto&& [constantName, pushConstant] : m_pushConstants)
        {
            auto& constantData = yaml["PushConstants"][constantName];

            uint32_t index = 0;
            for (auto&& [memberName, data] : pushConstant.dataPointers)
            {
                auto& member = constantData[index++];
                if (!member.IsDefined() || member.IsNull())
                {
                    EK_CORE_ERROR("Failed to deserialize member at index {0}, node '{1}' is not defined in material '{2}'", index-1, memberName, path.string());
                }
                else
                {
				    auto& memberData = member["Data"];
                    try
                    {
                        switch (data.type)
                        {
                            case DataType::FLOAT:   SetData<float>(data.data, memberData);      break;
                            case DataType::FLOAT2:  SetData<glm::vec2>(data.data, memberData);  break;
                            case DataType::FLOAT3:  SetData<glm::vec3>(data.data, memberData);  break;
                            case DataType::FLOAT4:  SetData<glm::vec4>(data.data, memberData);  break;
                            case DataType::MAT3:    SetData<glm::mat3>(data.data, memberData);  break;
                            case DataType::MAT4:    SetData<glm::mat4>(data.data, memberData);  break;
                            case DataType::INT:     SetData<int>(data.data, memberData);        break;
                            case DataType::INT2:    SetData<glm::ivec2>(data.data, memberData); break;
                            case DataType::INT3:    SetData<glm::ivec3>(data.data, memberData); break;
                            case DataType::INT4:    SetData<glm::ivec4>(data.data, memberData); break;
                            case DataType::BOOL:    SetData<bool>(data.data, memberData);       break;
                        }
                    }
                    catch (std::runtime_error e)
                    {
                        EK_CORE_ERROR("Failed to deserialize .ekmt file '{0}'\n     {1}", path.string(), e.what());
                    }
                }
            }
        }

        for (auto&& [samplerName, sampler] : m_samplers)
        {
            sampler.texture = nullptr;
            sampler.texturePath = "";

            auto pathNode = yaml["Samplers"][samplerName];
            if (pathNode.IsNull())
            {
                EK_CORE_TRACE("Sampler '{0}' not found in material '{1}'", samplerName, m_name);
                continue;
            }

            auto path = Path(pathNode.as<std::string>());
            if (path.empty())
            {
                EK_CORE_TRACE("Sampler '{0}' in material '{1}' has an empty path", samplerName, m_name);
                continue;
            }

            auto texPtr = Project::GetActive()->GetAssetLibrary()->GetTexture(path);
            if (texPtr == nullptr)
            {
                EK_CORE_TRACE("Sampler '{0}' in material '{1}' failed to load texture '{2}'", samplerName, m_name, path.string());
                continue;
            }

            sampler.texturePath = path;
            sampler.texture = texPtr;

            EK_CORE_TRACE("Sampler '{0}' in material '{1}' loaded texture '{2}'", samplerName, m_name, path.string());
        }
    }
}