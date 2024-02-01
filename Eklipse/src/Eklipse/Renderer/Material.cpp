#include "precompiled.h"
#include "Material.h"

#include <Eklipse/Renderer/Renderer.h>
#include <Eklipse/Platform/Vulkan/VKMaterial.h>
#include <Eklipse/Platform/OpenGL/GLMaterial.h>
#include <Eklipse/Assets/AssetManager.h>
#include <Eklipse/Utils/Yaml.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

namespace Eklipse
{
    static String DataTypeToString(ShaderDataType type)
    {
        switch (type)
        {
            case ShaderDataType::FLOAT:   return "float";
            case ShaderDataType::FLOAT2:  return "float2";
            case ShaderDataType::FLOAT3:  return "float3";
            case ShaderDataType::FLOAT4:  return "float4";
            case ShaderDataType::MAT3:    return "mat3";
            case ShaderDataType::MAT4:    return "mat4";
            case ShaderDataType::INT:     return "int";
            case ShaderDataType::INT2:    return "int2";
            case ShaderDataType::INT3:    return "int3";
            case ShaderDataType::INT4:    return "int4";
            case ShaderDataType::BOOL:    return "bool";
        }
        EK_ASSERT(false, "Unknown data type");
        return "UNKNOWN";
    }
    
    PushConstant::PushConstant(const PushConstant& other)
    {
        Copy(other);
    }
    PushConstant& PushConstant::operator=(const PushConstant& other)
    {
        if (this != &other)
        {
            this->Copy(other);
        }
        return *this;
    }
    void PushConstant::Copy(const PushConstant& other)
    {
        dataPointers = other.dataPointers;
        pushConstantSize = other.pushConstantSize;

        if (other.pushConstantData)
        {
            pushConstantData = std::make_unique<char[]>(pushConstantSize);
            std::copy(other.pushConstantData.get(), other.pushConstantData.get() + pushConstantSize, pushConstantData.get());
        }
    }

    Material::Material(const Path& path)
    {
        m_name = path.stem().string();
        if (!Deserialize(path))
        {
            EK_CORE_ERROR("Failed to deserialize material '{0}'", m_name);
        }
    }
    Material::Material(const Path& path, AssetHandle shaderHandle)
    {
        m_name = path.stem().string();

        SetShader(shaderHandle);
        if (!Serialize(path))
        {
            EK_CORE_ERROR("Failed to serialize material '{0}'", m_name);
        }
    }
    Ref<Material> Material::Create(const Path& path)
    {
        switch (Renderer::GetAPI())
        {
            case ApiType::Vulkan: return CreateRef<Vulkan::VKMaterial>(path);
            case ApiType::OpenGL: return CreateRef<OpenGL::GLMaterial>(path);
        }
        EK_ASSERT(false, "Material creation not implemented for current graphics API");
        return nullptr;
    }
    Ref<Material> Material::Create(const Path& path, AssetHandle shaderHandle)
    {
        switch (Renderer::GetAPI())
        {
            case ApiType::Vulkan: return CreateRef<Vulkan::VKMaterial>(path, shaderHandle);
            case ApiType::OpenGL: return CreateRef<OpenGL::GLMaterial>(path, shaderHandle);
        }
        EK_ASSERT(false, "Material creation not implemented for current graphics API");
        return nullptr;
    }
    
    template<typename T>
    static void SetData(void* dst, YAML::Node& node)
    {
        if (!node) 
        {
            EK_CORE_WARN("Failed to deserialize data, node '{0}' is null", node.Tag());
            dst = nullptr;
            return;
        }

        T data = node.as<T>();
        std::memcpy(dst, &data, sizeof(T));
    }

    void Material::Bind()
    {
        EK_PROFILE();
        m_shader->Bind();
    }
    void Material::ApplyChanges()
    {
        EK_PROFILE();
        EK_CORE_TRACE("Applying changes to material '{0}'", m_name);

        auto& materialPath = AssetManager::GetMetadata(Handle).FilePath;
        if (!Serialize(materialPath))
        {
            EK_CORE_ERROR("Failed to serialize material '{0}'", m_name);
            return;
        }
        if (!Deserialize(materialPath))
        {
            EK_CORE_ERROR("Failed to deserialize material '{0}'", m_name);
        }

        EK_CORE_DBG("Applied changes to material '{0}'", m_name);
    }

    void Material::SetShader(AssetHandle shaderHandle)
    {
        EK_PROFILE();
        EK_CORE_TRACE("Setting shader for material '{0}' to '{1}'", m_name, shaderHandle);

        EK_ASSERT(AssetManager::IsAssetHandleValid(shaderHandle), "Shader handle is not valid");
        if (m_shader && m_shader->Handle == shaderHandle)
        {
            return;
        }

        m_shader = AssetManager::GetAsset<Shader>(shaderHandle);
        m_shaderHandle = shaderHandle;
        m_pushConstants.clear();
        m_samplers.clear();

        for (auto&& [stage, reflection] : m_shader->GetReflections())
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
                m_samplers[samplerRef.name] = { samplerRef.binding, 0, nullptr };
            }
        }

        EK_CORE_DBG("Set shader for material '{0}' to '{1}'", m_name, m_shader->GetName());
    }
    void Material::OnShaderReloaded()
    {
        EK_PROFILE();
        EK_CORE_TRACE("Material::OnShaderReloaded for material '{0}'", m_name);

        // Applying new shader constants
        for (auto&& [stage, reflection] : m_shader->GetReflections())
        {
            for (auto& pushConstantRef : reflection.pushConstants)
            {
                // create new memory block
                PushConstant pushConstant{};
                pushConstant.pushConstantData = std::make_unique<char[]>(pushConstantRef.size);
                pushConstant.pushConstantSize = pushConstantRef.size;

                auto it = m_pushConstants.find(pushConstantRef.name);

                // if the push constant already exists, copy the available data over
                if (it != m_pushConstants.end())
                {
                    auto& oldPushConstant = it->second;

                    uint32_t offset = 0;
                    std::unordered_map<String, PushConstantData> dataPointers;
                    for (auto& member : pushConstantRef.members)
                    {
                        auto& dataPointer = dataPointers[member.name];
                        dataPointer = { pushConstant.pushConstantData.get() + offset, member.size, member.type };

                        // if the member already exists, copy the data over
                        auto it = oldPushConstant.dataPointers.find(member.name);
                        if (it != oldPushConstant.dataPointers.end())
                        {
                            std::memcpy(dataPointer.data, it->second.data, dataPointer.size);
                        }

                        offset += member.size;
                    }

                    oldPushConstant.dataPointers = dataPointers;
                    oldPushConstant.pushConstantData = std::move(pushConstant.pushConstantData);
                    oldPushConstant.pushConstantSize = pushConstant.pushConstantSize;
                }

                // otherwise, create a new push constant
                else
                {
                    uint32_t offset = 0;
                    for (auto& member : pushConstantRef.members)
                    {
                        pushConstant.dataPointers[member.name] = { pushConstant.pushConstantData.get() + offset, member.size, member.type };
                        offset += member.size;
                    }

                    auto& mPushConstant = m_pushConstants[pushConstantRef.name];
                    mPushConstant.dataPointers = pushConstant.dataPointers;
                    mPushConstant.pushConstantData = std::move(pushConstant.pushConstantData);
                    mPushConstant.pushConstantSize = pushConstant.pushConstantSize;
                }
            }

            for (auto& samplerRef : reflection.samplers)
            {
                auto it = m_samplers.find(samplerRef.name);
                if (it == m_samplers.end())
                {
                    it->second = { samplerRef.binding, 0, nullptr };
                }
            }
        }

        EK_CORE_DBG("Material::OnShaderReloaded for material '{0}'", m_name);
    }
    bool Material::Serialize(const Path& path)
    {
        EK_PROFILE();
        EK_CORE_TRACE("Serializing material '{0}' to '{1}'", m_name, path.string());

        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Name" << YAML::Value << m_name;
        out << YAML::Key << "Shader" << YAML::Value << m_shader->Handle;

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
                        case ShaderDataType::FLOAT:   out << *(float*)data.data;      break;
                        case ShaderDataType::FLOAT2:  out << *(glm::vec2*)data.data;  break;
                        case ShaderDataType::FLOAT3:  out << *(glm::vec3*)data.data;  break;
                        case ShaderDataType::FLOAT4:  out << *(glm::vec4*)data.data;  break;
                        case ShaderDataType::MAT3:    out << *(glm::mat3*)data.data;  break;
                        case ShaderDataType::MAT4:    out << *(glm::mat4*)data.data;  break;
                        case ShaderDataType::INT:     out << *(int*)data.data;        break;
                        case ShaderDataType::INT2:    out << *(glm::ivec2*)data.data; break;
                        case ShaderDataType::INT3:    out << *(glm::ivec3*)data.data; break;
                        case ShaderDataType::INT4:    out << *(glm::ivec4*)data.data; break;
                        case ShaderDataType::BOOL:    out << *(bool*)data.data;       break;
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
                out << YAML::Key << samplerName << YAML::Value << sampler.textureHandle;
            }
            out << YAML::EndMap;
        }

        out << YAML::EndMap;

        auto& materialPath = AssetManager::GetMetadata(Handle).FilePath;
        std::ofstream fout(materialPath);
        fout << out.c_str();

        EK_CORE_DBG("Serialized material '{0}' to '{1}'", m_name, path.string());
        return true;
    }
    bool Material::Deserialize(const Path& path)
    {
        EK_PROFILE();
        EK_CORE_TRACE("Deserializing material '{0}' from '{1}'", m_name, path.string());

        YAML::Node yaml;
        try
        {
            auto& materialPath = AssetManager::GetMetadata(Handle).FilePath;
            yaml = YAML::LoadFile(materialPath.string());
        }
        catch (std::runtime_error e)
        {
            EK_CORE_ERROR("Failed to load .ekmt file '{0}'\n     {1}", path.string(), e.what());
            return false;
        }

        if (!yaml["Name"])
        {
            EK_CORE_ERROR("Material file '{0}' is missing a name", path.string());
            return false;
        }

        m_name = yaml["Name"].as<String>();

        if (!yaml["Shader"])
        {
            EK_CORE_ERROR("Material file '{0}' is missing a shader path", path.string());
            return false;
        }

        AssetHandle shaderHandle = yaml["Shader"].as<AssetHandle>();
        SetShader(shaderHandle);

        auto& constantsNode = yaml["PushConstants"];
        if (!constantsNode.IsDefined() || constantsNode.IsNull())
        {
            EK_CORE_ERROR("Material file '{0}' is missing a push constants node", path.string());
        }
        else
        {
            for (YAML::iterator it = constantsNode.begin(); it != constantsNode.end(); ++it) 
            {
                auto& pushConstant = it->second;
                String& constantName = it->first.as<String>();
                for (YAML::iterator it = pushConstant.begin(); it != pushConstant.end(); ++it)
                {
                    auto& member = *it;
                    String memberName = member["Name"].as<String>();
                    if (!member.IsDefined() || member.IsNull())
                    {
                        EK_CORE_ERROR("Failed to deserialize. Member '{1}' is not defined in material '{2}'", memberName, path.string());
                    }
                    else
                    {
                        auto& memberData = member["Data"];
                        void* data = m_pushConstants[constantName].dataPointers[memberName].data;
                        String memberType = member["Type"].as<String>();

                        try
                        {
                            if (memberType == "float")          SetData<float>(data, memberData);
                            else if (memberType == "float2")    SetData<glm::vec2>(data, memberData);
                            else if (memberType == "float3")    SetData<glm::vec3>(data, memberData);
                            else if (memberType == "float4")    SetData<glm::vec4>(data, memberData);
                            else if (memberType == "mat3")      SetData<glm::mat3>(data, memberData);
                            else if (memberType == "mat4")      SetData<glm::mat4>(data, memberData);
                            else if (memberType == "int")       SetData<int>(data, memberData);
                            else if (memberType == "int2")      SetData<glm::ivec2>(data, memberData);
                            else if (memberType == "int3")      SetData<glm::ivec3>(data, memberData);
                            else if (memberType == "int4")      SetData<glm::ivec4>(data, memberData);
                            else if (memberType == "bool")      SetData<bool>(data, memberData);
                        }
                        catch (std::runtime_error e)
                        {
                            EK_CORE_ERROR("Failed to deserialize .ekmt file '{0}'\n     {1}", path.string(), e.what());
                        }
                    }
                }
            }
        }

        for (auto&& [samplerName, sampler] : m_samplers)
        {
            sampler.texture = nullptr;

            AssetHandle textureHandle = TryDeserailize<AssetHandle>(yaml["Samplers"], samplerName, -1);
            auto texture = AssetManager::GetAsset<Texture2D>(textureHandle);
            if (texture == nullptr)
            {
                EK_CORE_TRACE("Sampler '{0}' in material '{1}' failed to load texture {2}", samplerName, m_name, textureHandle);
                continue;
            }

            sampler.textureHandle = textureHandle;
            sampler.texture = texture;

            EK_CORE_TRACE("Sampler '{0}' in material '{1}' loaded texture '{2}'", samplerName, m_name, path.string());
        }

        return true;
    }

    const String& Material::GetName() const
    {
        return m_name;
    }
    const Ref<Shader> Material::GetShader() const
    {
        return m_shader;
    }
    AssetHandle& Material::GetShaderHandle() 
    {
        return m_shaderHandle;
    }
    const PushConstantMap& Material::GetPushConstants() const
    {
        return m_pushConstants;
    }
    Sampler2DMap& Material::GetSamplers()
    {
        return m_samplers;
    }
    bool Material::IsValid() const
    {
        return m_shader != nullptr && m_shader->IsValid();
    }
}