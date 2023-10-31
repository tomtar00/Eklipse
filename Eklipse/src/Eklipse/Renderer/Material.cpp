#include "precompiled.h"
#include "Material.h"

#include <Eklipse/Scene/Assets.h>
#include <Eklipse/Renderer/Renderer.h>
#include <Eklipse/Platform/Vulkan/VKMaterial.h>
#include <Eklipse/Platform/OpenGL/GLMaterial.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <yaml-cpp/yaml.h>

namespace YAML {

    template<>
    struct convert<glm::vec2>
    {
        static Node encode(const glm::vec2& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }
        static bool decode(const Node& node, glm::vec2& rhs)
        {
            if (!node.IsSequence() || node.size() != 2)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            return true;
        }
    };
    template<>
    struct convert<glm::vec3>
    {
        static Node encode(const glm::vec3& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }
        static bool decode(const Node& node, glm::vec3& rhs)
        {
            if (!node.IsSequence() || node.size() != 3)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            return true;
        }
    };
    template<>
    struct convert<glm::vec4>
    {
        static Node encode(const glm::vec4& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.push_back(rhs.w);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }
        static bool decode(const Node& node, glm::vec4& rhs)
        {
            if (!node.IsSequence() || node.size() != 4)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            rhs.w = node[3].as<float>();
            return true;
        }
    };

    template<>
    struct convert<glm::mat3>
    {
        static Node encode(const glm::mat3& rhs)
        {
            Node node;
            node.push_back(rhs[0]);
            node.push_back(rhs[1]);
            node.push_back(rhs[2]);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }
        static bool decode(const Node& node, glm::mat3& rhs)
        {
            rhs[0] = node[0].as<glm::vec3>();
            rhs[1] = node[1].as<glm::vec3>();
            rhs[2] = node[2].as<glm::vec3>();
            return true;
        }
    };
    template<>
    struct convert<glm::mat4>
    {
        static Node encode(const glm::mat4& rhs)
        {
            Node node;
            node.push_back(rhs[0]);
            node.push_back(rhs[1]);
            node.push_back(rhs[2]);
            node.push_back(rhs[3]);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }
        static bool decode(const Node& node, glm::mat4& rhs)
        {
            rhs[0] = node[0].as<glm::vec4>();
            rhs[1] = node[1].as<glm::vec4>();
            rhs[2] = node[2].as<glm::vec4>();
            rhs[3] = node[3].as<glm::vec4>();
            return true;
        }
    };

    template<>
    struct convert<glm::ivec2>
    {
        static Node encode(const glm::ivec2& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }
        static bool decode(const Node& node, glm::ivec2& rhs)
        {
            if (!node.IsSequence() || node.size() != 2)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            return true;
        }
    };
    template<>
    struct convert<glm::ivec3>
    {
        static Node encode(const glm::ivec3& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }
        static bool decode(const Node& node, glm::ivec3& rhs)
        {
            if (!node.IsSequence() || node.size() != 3)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            return true;
        }
    };
    template<>
    struct convert<glm::ivec4>
    {
        static Node encode(const glm::ivec4& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.push_back(rhs.w);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }
        static bool decode(const Node& node, glm::ivec4& rhs)
        {
            if (!node.IsSequence() || node.size() != 4)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            rhs.w = node[3].as<float>();
            return true;
        }
    };
}

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
        T data = node.as<T>();
        dst = &data;
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
        return out;
    }
    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
        return out;
    }
    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
        return out;
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::mat3& v)
    {
		out << YAML::Flow;
		out << YAML::BeginSeq << v[0] << v[1] << v[2] << YAML::EndSeq;
		return out;
	}
    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::mat4& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v[0] << v[1] << v[2] << v[3] << YAML::EndSeq;
        return out;
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::ivec2& v)
    {
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return out;
	}
    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::ivec3& v)
    {
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}
    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::ivec4& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
        return out;
    }

    Ref<Material> Eklipse::Material::Create(const std::filesystem::path& path)
    {
        auto apiType = Renderer::GetAPI();
        switch (apiType)
        {
            case ApiType::Vulkan: return CreateRef<Vulkan::VKMaterial>(path);
            case ApiType::OpenGL: return CreateRef<OpenGL::GLMaterial>(path);
        }
        EK_ASSERT(false, "API {0} not implemented for Material creation", int(apiType));
        return nullptr;
    }

    Material::Material(const std::filesystem::path& path)
    {
        Deserialize(path);

        m_name = path.stem().string();
        Serialize(path);
    }

    void Material::SetSampler(const std::string& samplerName, const std::filesystem::path& texturePath)
    {
        EK_ASSERT(m_samplers.find(samplerName) != m_samplers.end(), "Sampler '{0}' not found in material {1}", samplerName, m_name);
		m_samplers[samplerName] = texturePath;
    }

    void Material::Bind()
    {
        m_shader->Bind();
    }
    void Material::Dispose()
    {
        m_shader->Dispose();
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
                m_samplers[samplerRef.name] = "";
            }
        }
    }

    void Material::Serialize(const std::filesystem::path& path)
    {
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Name" << YAML::Value << m_name;
        out << YAML::Key << "Shader" << YAML::Value << m_shader->GetPath();

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
            for (auto&& [samplerName, texturePath] : m_samplers)
            {
				out << YAML::Key << samplerName << YAML::Value << texturePath.string();
			}
			out << YAML::EndMap;
        }

        out << YAML::EndMap;

        std::ofstream fout(path);
        fout << out.c_str();
    }
    void Material::Deserialize(const std::filesystem::path& path)
    {
        YAML::Node data;
        try
        {
            data = YAML::LoadFile(path.string());
        }
        catch (YAML::ParserException e)
        {
            EK_CORE_ERROR("Failed to load .ekmat file '{0}'\n     {1}", path.string(), e.what());
            return;
        }

        if (!data["Name"])
        {
			EK_CORE_ERROR("Material file '{0}' is missing a name", path.string());
			return;
		}

        m_name = data["Name"].as<std::string>();
		m_path = path;

        if (!data["Shader"])
        {
            EK_CORE_ERROR("Material file '{0}' is missing a shader path", path.string());
            return;
        }

        m_shader = Assets::GetShader(data["Shader"].as<std::string>());
        SetShader(m_shader);

        for (auto&& [constantName, pushConstant] : m_pushConstants)
        {
            auto& constantData = data[constantName];
            for (auto&& [memberName, data] : pushConstant.dataPointers)
            {
				auto& memberData = constantData[memberName];
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
                    EK_ASSERT("Failed to deserialize .ekmat file '{0}'\n     {1}", path.string(), e.what());
                    return;
                }
            }
        }

        m_samplers.clear();
        for (auto&& [samplerName, texturePath] : m_samplers)
        {
            texturePath = data["Samplers"][samplerName].as<std::string>();

            if (texturePath.empty()) continue;
            m_sampledTextures.push_back(Assets::GetTexture(texturePath.string()));
        }
    }
}