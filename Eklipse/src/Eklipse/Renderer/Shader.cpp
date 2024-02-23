#include "precompiled.h"
#include "Shader.h"
#include "Material.h"

#include <Eklipse/Renderer/Renderer.h>
#include <Eklipse/Assets/AssetManager.h>
#include <Eklipse/Platform/OpenGL/GLShader.h>
#include <Eklipse/Platform/Vulkan/VKShader.h>
#include <Eklipse/Utils/File.h>

#include <spirv_cross.hpp>
#include <spirv_glsl.hpp>

namespace Eklipse
{
    static void CreateCacheDirectoryIfNeeded(const String& cacheDirectory)
    {
        EK_CORE_PROFILE();
        if (!fs::exists(cacheDirectory))
            fs::create_directories(cacheDirectory);
    }
    static const char* VKShaderStageCachedVulkanFileExtension(const ShaderStage stage)
    {
        EK_CORE_PROFILE();
        switch (stage)
        {
        case ShaderStage::VERTEX:    return ".cached_vulkan.vert";
        case ShaderStage::FRAGMENT:  return ".cached_vulkan.frag";
        }
        EK_ASSERT(false, "Unknown shader stage!");
        return "";
    }
    static size_t GetSizeOfSpirvType(const spirv_cross::SPIRType type, const String& name)
    {
        EK_CORE_PROFILE();
        switch (type.basetype)
        {
        case spirv_cross::SPIRType::Float: return type.vecsize * sizeof(float);
        case spirv_cross::SPIRType::Int:
        case spirv_cross::SPIRType::UInt: return type.vecsize * sizeof(int);
        }
        EK_ASSERT(false, "Unkown SPIR-V type! Type: {0} Name: {1}", (uint32_t)type.basetype, name);
        return 0;
    }
    static ShaderDataType SpirvTypeToDataType(const spirv_cross::SPIRType type)
    {
        EK_CORE_PROFILE();
        if (type.basetype == spirv_cross::SPIRType::Image && type.image.dim == spv::Dim2D) return ShaderDataType::SAMPLER2D;
        if (type.columns == 3 && type.vecsize == 3) return ShaderDataType::MAT3;
        if (type.columns == 4 && type.vecsize == 4) return ShaderDataType::MAT4;

        if (type.basetype == spirv_cross::SPIRType::Boolean) return ShaderDataType::BOOL;
        if (type.basetype == spirv_cross::SPIRType::Float)
        {
            if (type.vecsize == 1) return ShaderDataType::FLOAT;
            if (type.vecsize == 2) return ShaderDataType::FLOAT2;
            if (type.vecsize == 3) return ShaderDataType::FLOAT3;
            if (type.vecsize == 4) return ShaderDataType::FLOAT4;
        }
        if (type.basetype == spirv_cross::SPIRType::Int)
        {
            if (type.vecsize == 1) return ShaderDataType::INT;
            if (type.vecsize == 2) return ShaderDataType::INT2;
            if (type.vecsize == 3) return ShaderDataType::INT3;
            if (type.vecsize == 4) return ShaderDataType::INT4;
        }
        if (type.basetype == spirv_cross::SPIRType::UInt)
        {
            if (type.vecsize == 1) return ShaderDataType::UINT;
            if (type.vecsize == 2) return ShaderDataType::UINT2;
            if (type.vecsize == 3) return ShaderDataType::UINT3;
            if (type.vecsize == 4) return ShaderDataType::UINT4;
        }
        if (type.basetype == spirv_cross::SPIRType::Struct) return ShaderDataType::STRUCT;

        EK_ASSERT(false, "Unkown SPIR-V type! Type: {0}", (uint32_t)type.basetype);
        return ShaderDataType::NONE;
    }
    String ShaderDataTypeToString(ShaderDataType type)
    {
        EK_CORE_PROFILE();
        switch (type)
        {
        case ShaderDataType::NONE:     return "none";
        case ShaderDataType::BOOL:     return "bool";
        case ShaderDataType::INT:      return "int";
        case ShaderDataType::INT2:     return "int2";
        case ShaderDataType::INT3:     return "int3";
        case ShaderDataType::INT4:     return "int4";
        case ShaderDataType::UINT:     return "uint";
        case ShaderDataType::UINT2:    return "uint2";
        case ShaderDataType::UINT3:    return "uint3";
        case ShaderDataType::UINT4:    return "uint4";
        case ShaderDataType::FLOAT:    return "float";
        case ShaderDataType::FLOAT2:   return "float2";
        case ShaderDataType::FLOAT3:   return "float3";
        case ShaderDataType::FLOAT4:   return "float4";
        case ShaderDataType::MAT3:     return "mat3";
        case ShaderDataType::MAT4:     return "mat4";
        case ShaderDataType::SAMPLER2D:return "sampler2D";
        case ShaderDataType::STRUCT:   return "struct";
        }
        EK_ASSERT(false, "Unkown data type!");
        return "none";
    }

    ShaderStage StringToShaderStage(const String& stage)
    {
        EK_CORE_PROFILE();
        if (stage == "vertex")   return ShaderStage::VERTEX;
        if (stage == "fragment") return ShaderStage::FRAGMENT;
        if (stage == "compute")  return ShaderStage::COMPUTE;

        EK_ASSERT(false, "Wrong shader stage");
        return ShaderStage::NONE;
    }
    String ShaderStageToString(ShaderStage stage)
    {
        EK_CORE_PROFILE();
        switch (stage)
        {
        case ShaderStage::VERTEX:   return "vertex";
        case ShaderStage::FRAGMENT: return "fragment";
        case ShaderStage::COMPUTE:  return "compute";
        }
        EK_ASSERT(false, "Wrong shader stage");
        return "none";
    }
    uint32_t ShaderStageToShaderC(const ShaderStage stage)
    {
        EK_CORE_PROFILE();
        switch (stage)
        {
        case ShaderStage::VERTEX:   return shaderc_glsl_vertex_shader;
        case ShaderStage::FRAGMENT: return shaderc_glsl_fragment_shader;
        case ShaderStage::COMPUTE:  return shaderc_glsl_compute_shader;
        }
        EK_ASSERT(false, "Unkown shader stage!");
        return 0;
    }

    Shader::Shader(const String& vertexSource, const String& fragmentSource, const AssetHandle handle) : m_isValid(false)
    {
    }
    Shader::Shader(const Path& filePath, const AssetHandle handle) : m_isValid(false)
    {
    }

    Ref<Shader> Shader::Create(const Path& filePath, const AssetHandle handle)
    {
        EK_CORE_PROFILE();
        switch (Renderer::GetAPI())
        {
            case ApiType::Vulkan: return CreateRef<Vulkan::VKShader>(filePath, handle);
            case ApiType::OpenGL: return CreateRef<OpenGL::GLShader>(filePath, handle);
        }
        EK_ASSERT(false, "Shader creation not implemented for current graphics API");
        return nullptr;
    }
    Ref<Shader> Shader::Create(const String& vertexSource, const String& fragmentSource, const AssetHandle handle)
    {
        EK_CORE_PROFILE();
        switch (Renderer::GetAPI())
        {
            case ApiType::Vulkan: return CreateRef<Vulkan::VKShader>(vertexSource, fragmentSource, handle);
            case ApiType::OpenGL: return CreateRef<OpenGL::GLShader>(vertexSource, fragmentSource, handle);
        }
        EK_ASSERT(false, "Shader creation not implemented for current graphics API");
        return nullptr;
    }

    StageSourceMap Shader::Setup(const Path& shaderPath)
    {
        EK_CORE_PROFILE();
        EK_CORE_TRACE("Setting up shader '{0}'", Name);

        CreateCacheDirectoryIfNeeded(GetCacheDirectoryPath());

        String source = FileUtilities::ReadFileFromPath(shaderPath);
        Name = shaderPath.stem().string();

        EK_CORE_DBG("Set up shader '{0}'", Name);
        return PreProcess(source);
    }
    bool Shader::Recompile(const Path& shaderPath)
    {
        EK_CORE_PROFILE();
        EK_CORE_TRACE("Recompiling shader '{0}'", Name);

        Dispose();
        Compile(shaderPath, true);

        if (m_isValid)
        {
            // reload all materials that use this shader
            // TODO: this should not rely on AssetManager, make some dependency tracker instead
            for (auto&& [handle, asset] : AssetManager::GetLoadedAssets())
            {
                if (asset->GetType() != AssetType::Material)
                    continue;

                Ref<Material> material = AssetManager::GetAsset<Material>(handle);
                if (material->GetShaderHandle() == Handle)
                {
                    EK_CORE_TRACE("Reloading material {0}, because shader {1} has been recompiled", material->Handle, Handle);
                    auto& metadata = AssetManager::GetMetadata(handle);
                    material->OnShaderReloaded();
                    material->ApplyChanges(metadata.FilePath);
                }
            }
        }

        EK_CORE_DBG("Recompiled shader '{0}'. Success: {1}", Name, m_isValid);

        return m_isValid;
    }

    const StageReflectionMap& Shader::GetReflections() const
    {
        return m_reflections;
    }
    const ShaderReflection& Shader::GetVertexReflection()
    {
        return m_reflections[ShaderStage::VERTEX];
    }
    const ShaderReflection& Shader::GetFragmentReflection()
    {
        return m_reflections[ShaderStage::FRAGMENT];
    }
    bool Shader::IsValid() const
    {
        return m_isValid;
    }

    void Shader::Reflect(const StageSpirvMap& shaderStagesData, const String& shaderName)
    {
        EK_CORE_PROFILE();
        EK_CORE_DBG("Shader::Reflect - {0}", shaderName);
        uint32_t push_const_offset = 0;
        for (auto&& [stage, shaderData] : shaderStagesData)
        {
            EK_CORE_TRACE("Stage: {0}", ShaderStageToString(stage));
            spirv_cross::Compiler compiler(shaderData);
            spirv_cross::ShaderResources resources = compiler.get_shader_resources();

            ShaderReflection reflection;
            EK_CORE_TRACE("Inputs:");
            size_t offset = 0;
            for (size_t inputIndex = 0; inputIndex < resources.stage_inputs.size(); ++inputIndex)
            {
                auto& resource = resources.stage_inputs[inputIndex];
                const auto& name = compiler.get_name(resource.id);
                const auto& inputType = compiler.get_type(resource.base_type_id);
                uint32_t location = compiler.get_decoration(resource.id, spv::DecorationLocation);
                if (location > reflection.maxLocation)
                    reflection.maxLocation = location;
                size_t size = GetSizeOfSpirvType(inputType, name);
                ShaderDataType type = SpirvTypeToDataType(inputType);
                EK_CORE_TRACE("\tName: {0}", name);
                EK_CORE_TRACE("\tLocation: {0}", location);
                EK_CORE_TRACE("\tSize: {0}", size);
                EK_CORE_TRACE("\tOffset: {0}", offset);
                reflection.inputs.push_back({ name, location, size, offset, type });
                offset += size;
            }
            EK_CORE_TRACE("Outputs:");
            for (const auto& resource : resources.stage_outputs)
            {
                const auto& name = compiler.get_name(resource.id);
                const auto& outputType = compiler.get_type(resource.base_type_id);
                uint32_t location = compiler.get_decoration(resource.id, spv::DecorationLocation);
                if (location > reflection.maxLocation)
                    reflection.maxLocation = location;
                size_t size = GetSizeOfSpirvType(outputType, name);
                ShaderDataType type = SpirvTypeToDataType(outputType);
                EK_CORE_TRACE("\tName: {0}", name);
                EK_CORE_TRACE("\tLocation: {0}", location);
                EK_CORE_TRACE("\tSize: {0}", size);
                reflection.outputs.push_back({ name, location, size, type });
            }
            EK_CORE_TRACE("Uniform buffers:");
            for (const auto& resource : resources.uniform_buffers)
            {
                auto& name = compiler.get_name(resource.id);
                const auto& bufferType = compiler.get_type(resource.base_type_id);
                size_t bufferSize = compiler.get_declared_struct_size(bufferType);
                uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
                EK_CORE_TRACE("\tName: {0}", name);
                EK_CORE_TRACE("\tSize: {0}", bufferSize);
                EK_CORE_TRACE("\tBinding: {0}", binding);
                ShaderUniformBuffer uniformBuffer = { name, bufferSize, binding };
                for (size_t memberIndex = 0; memberIndex < bufferType.member_types.size(); ++memberIndex)
                {
                    const auto& name = compiler.get_member_name(bufferType.self, memberIndex);
                    const auto& memberType = compiler.get_type(bufferType.member_types[memberIndex]);
                    size_t memberSize = compiler.get_declared_struct_member_size(bufferType, memberIndex);
                    uint32_t memberOffset = compiler.get_member_decoration(bufferType.self, memberIndex, spv::DecorationOffset);
                    ShaderDataType type = SpirvTypeToDataType(memberType);
                    EK_CORE_TRACE("\t\tName: {0}", name);
                    EK_CORE_TRACE("\t\tSize: {0}", memberSize);
                    EK_CORE_TRACE("\t\tOffset: {0}", memberOffset);
                    EK_CORE_TRACE("\t\tType: {0}", ShaderDataTypeToString(type));
                    uniformBuffer.members.push_back({ name, memberSize, memberOffset, type });
                }
                reflection.uniformBuffers.push_back(uniformBuffer);

                Renderer::CreateUniformBuffer(name, bufferSize, binding);
            }
            EK_CORE_TRACE("Push constants:");
            for (const auto& pushConstant : resources.push_constant_buffers)
            {
                auto& name = compiler.get_name(pushConstant.id);
                const auto& bufferType = compiler.get_type(pushConstant.base_type_id);
                size_t bufferSize = compiler.get_declared_struct_size(bufferType);
                EK_CORE_TRACE("\tName: {0}", name);
                EK_CORE_TRACE("\tSize: {0}", bufferSize);
                EK_CORE_TRACE("\tOffset: {0}", push_const_offset);
                ShaderPushConstant pushConstant = { name, bufferSize, push_const_offset };
                push_const_offset += bufferSize;
                for (size_t memberIndex = 0; memberIndex < bufferType.member_types.size(); ++memberIndex)
                {
                    const auto& name = compiler.get_member_name(bufferType.self, memberIndex);
                    const auto& memberType = compiler.get_type(bufferType.member_types[memberIndex]);
                    size_t memberSize = compiler.get_declared_struct_member_size(bufferType, memberIndex);
                    uint32_t memberOffset = compiler.get_member_decoration(bufferType.self, memberIndex, spv::DecorationOffset);
                    ShaderDataType type = SpirvTypeToDataType(memberType);
                    EK_CORE_TRACE("\t\tName: {0}", name);
                    EK_CORE_TRACE("\t\tSize: {0}", memberSize);
                    EK_CORE_TRACE("\t\tOffset: {0}", memberOffset);
                    EK_CORE_TRACE("\t\tType: {0}", ShaderDataTypeToString(type));
                    pushConstant.members.push_back({ name, memberSize, memberOffset, type });
                }
                reflection.pushConstants.push_back(pushConstant);
            }
            EK_CORE_TRACE("Samplers:");
            for (const auto& resource : resources.sampled_images)
            {
                const auto& type = compiler.get_type(resource.base_type_id);
                uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
                EK_CORE_TRACE("\tName: {0}", resource.name);
                EK_CORE_TRACE("\tBinding: {0}", binding);
                reflection.samplers.push_back({ resource.name, binding });
            }
            EK_CORE_TRACE("Storage buffers:");
            for (const auto& resource : resources.storage_buffers)
            {
                auto& name = compiler.get_name(resource.id);
                const auto& bufferType = compiler.get_type(resource.base_type_id);
                uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
                size_t bufferSize = compiler.get_declared_struct_size(bufferType);
                EK_CORE_TRACE("\tName: {0}", name);
                EK_CORE_TRACE("\tSize: {0}", bufferSize);
                EK_CORE_TRACE("\tBinding: {0}", binding);
                ShaderStorageBuffer storageBuffer = { name, bufferSize, binding };
                for (size_t memberIndex = 0; memberIndex < bufferType.member_types.size(); ++memberIndex)
                {
                    const auto& name = compiler.get_member_name(bufferType.self, memberIndex);
                    const auto& memberType = compiler.get_type(bufferType.member_types[memberIndex]);
                    size_t memberSize = compiler.get_declared_struct_member_size(bufferType, memberIndex);
                    uint32_t memberOffset = compiler.get_member_decoration(bufferType.self, memberIndex, spv::DecorationOffset);
                    ShaderDataType type = SpirvTypeToDataType(memberType);
                    EK_CORE_TRACE("\t\tName: {0}", name);
                    EK_CORE_TRACE("\t\tSize: {0}", memberSize);
                    EK_CORE_TRACE("\t\tOffset: {0}", memberOffset);
                    EK_CORE_TRACE("\t\tType: {0}", ShaderDataTypeToString(type));
                    storageBuffer.members.push_back({ name, memberSize, memberOffset, type });
                }
                reflection.storageBuffers.push_back(storageBuffer);
            }
            m_reflections[stage] = reflection;
        }
    }
    bool Shader::CompileOrGetVulkanBinaries(const Path& shaderPath, const StageSourceMap& shaderSources, bool forceCompile)
    {
        EK_CORE_PROFILE();
        EK_CORE_TRACE("Compiling vulkan binaries for shader '{0}'", Name);

        bool success = true;
        shaderc::Compiler compiler;
        shaderc::CompileOptions options;
        options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);
        options.SetAutoMapLocations(true);
        options.SetIncluder(Unique<shaderc::CompileOptions::IncluderInterface>(new ShaderIncluder));

        //! Strips reflection info
        //options.SetOptimizationLevel(shaderc_optimization_level_performance);

        Path cacheDirectory = "Assets/Cache/Shader/Vulkan";
        CreateCacheDirectoryIfNeeded(cacheDirectory.string());

        auto& shaderData = m_vulkanSPIRV;
        shaderData.clear();
        for (auto&& [stage, source] : shaderSources)
        {
            Path cachedPath = cacheDirectory / (Name + VKShaderStageCachedVulkanFileExtension(stage));

            std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
            if (!forceCompile && in.is_open())
            {
                EK_CORE_TRACE("Reading Vulkan shader '{0}' at stage '{1}' cache binaries from path: '{2}'", Handle, ShaderStageToString(stage), cachedPath.string());

                in.seekg(0, std::ios::end);
                auto size = in.tellg();
                in.seekg(0, std::ios::beg);

                auto& data = shaderData[stage];
                data.resize(size / sizeof(uint32_t));
                in.read((char*)data.data(), size);
            }
            else
            {
                EK_CORE_TRACE("Compiling shader '{0}' at stage '{1}' to Vulkan binaries", Handle, ShaderStageToString(stage));

                shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, (shaderc_shader_kind)ShaderStageToShaderC(stage), shaderPath.string().c_str(), options);
                if (module.GetCompilationStatus() != shaderc_compilation_status_success)
                {
                    success = false;
                    EK_CORE_ERROR("Failed to compile shader {0} at stage {1}. {2}", Handle, ShaderStageToString(stage), module.GetErrorMessage());
                }
                else
                {
                    shaderData[stage] = Vec<uint32_t>(module.cbegin(), module.cend());

                    EK_CORE_TRACE("Writing Vulkan shader '{0}' at stage '{1}' cache binaries to path: '{2}'", Handle, ShaderStageToString(stage), cachedPath.string());
                    std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
                    if (out.is_open())
                    {
                        auto& data = shaderData[stage];
                        out.write((char*)data.data(), data.size() * sizeof(uint32_t));
                        out.flush();
                        out.close();
                    }
                    else
                    {
                        success = false;
                        EK_CORE_ERROR("Failed to write Vulkan shader '{0}' at stage '{1}' cache binaries to path: '{2}'", Handle, ShaderStageToString(stage), cachedPath.string());
                    }
                }
            }
        }

        if (success)
        {
            Reflect(m_vulkanSPIRV, Name);
            EK_CORE_DBG("Compiled vulkan binaries for shader '{0}'", Name);
        }
        m_isValid = success;
        return success;
    }
    StageSourceMap Shader::PreProcess(const String& source) const
    {
        EK_CORE_PROFILE();
        EK_CORE_TRACE("Preprocessing shader source in shader '{0}'", Name);

        std::unordered_map<ShaderStage, String> shaderSources;

        const char* typeToken = "#stage";
        size_t typeTokenLength = strlen(typeToken);
        size_t pos = source.find(typeToken, 0);
        while (pos != String::npos)
        {
            size_t eol = source.find_first_of("\r\n", pos);
            EK_ASSERT(eol != String::npos, "Shader stage syntax error (1)");
            size_t begin = pos + typeTokenLength + 1;
            String type = source.substr(begin, eol - begin);
            EK_ASSERT(StringToShaderStage(type) != ShaderStage::NONE, "Invalid shader stage specified");

            size_t nextLinePos = source.find_first_not_of("\r\n", eol);
            EK_ASSERT(nextLinePos != String::npos, "Shader stage syntax error (2)");
            pos = source.find(typeToken, nextLinePos);

            shaderSources[StringToShaderStage(type)] = (pos == String::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
        }

        EK_CORE_DBG("Preprocessed shader source in shader '{0}'", Name);

        return shaderSources;
    }
    shaderc_include_result* ShaderIncluder::GetInclude(const char* requested_source, shaderc_include_type type, const char* requesting_source, size_t include_depth)
    {
        // Load the requested file
        Path requested_source_path = Path(requesting_source).parent_path() / requested_source;
        std::ifstream file(requested_source_path);
        if (!file.is_open()) {
            EK_CORE_ERROR("Failed to open included file: {}", requested_source_path.string());
            return nullptr;
        }

        // Read the file content
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string source_string = buffer.str();

        // Create shaderc_include_result
        auto* result = new shaderc_include_result;
        result->content = strdup(source_string.c_str());
        result->content_length = source_string.size();
        result->source_name = strdup(requested_source);
        result->source_name_length = strlen(requested_source);
        result->user_data = nullptr;

        return result;
    }
    void ShaderIncluder::ReleaseInclude(shaderc_include_result* data)
    {
        free((void*)data->content);
        free((void*)data->source_name);
        delete data;
    }
}