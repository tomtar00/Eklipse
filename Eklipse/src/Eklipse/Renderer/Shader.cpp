#include "precompiled.h"
#include "Shader.h"
#include "Material.h"

#include <Eklipse/Renderer/Renderer.h>
#include <Eklipse/Assets/AssetManager.h>
#include <Eklipse/Platform/OpenGL/GLShader.h>
#include <Eklipse/Platform/Vulkan/VKShader.h>
#include <Eklipse/Utils/File.h>

#include <shaderc/shaderc.hpp>
#include <spirv_cross.hpp>
#include <spirv_glsl.hpp>

namespace Eklipse
{
    static void CreateCacheDirectoryIfNeeded(const String& cacheDirectory)
    {
        if (!fs::exists(cacheDirectory))
            fs::create_directories(cacheDirectory);
    }
    static const char* VKShaderStageCachedVulkanFileExtension(const ShaderStage stage)
    {
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
        if (type.basetype == spirv_cross::SPIRType::Int || type.basetype == spirv_cross::SPIRType::UInt)
        {
            if (type.vecsize == 1) return ShaderDataType::INT;
            if (type.vecsize == 2) return ShaderDataType::INT2;
            if (type.vecsize == 3) return ShaderDataType::INT3;
            if (type.vecsize == 4) return ShaderDataType::INT4;
        }

        EK_ASSERT(false, "Unkown SPIR-V type! Type: {0}", (uint32_t)type.basetype);
        return ShaderDataType::NONE;
    }
    static String DataTypeToString(ShaderDataType type)
    {
        switch (type)
        {
        case ShaderDataType::NONE:     return "none";
        case ShaderDataType::BOOL:     return "bool";
        case ShaderDataType::INT:      return "int";
        case ShaderDataType::INT2:     return "int2";
        case ShaderDataType::INT3:     return "int3";
        case ShaderDataType::INT4:     return "int4";
        case ShaderDataType::FLOAT:    return "float";
        case ShaderDataType::FLOAT2:   return "float2";
        case ShaderDataType::FLOAT3:   return "float3";
        case ShaderDataType::FLOAT4:   return "float4";
        case ShaderDataType::MAT3:     return "mat3";
        case ShaderDataType::MAT4:     return "mat4";
        case ShaderDataType::SAMPLER2D:return "sampler2D";
        }
        EK_ASSERT(false, "Unkown data type!");
        return "none";
    }

    ShaderStage StringToShaderStage(const String& stage)
    {
        if (stage == "vertex")   return ShaderStage::VERTEX;
        if (stage == "fragment") return ShaderStage::FRAGMENT;
        if (stage == "compute")  return ShaderStage::COMPUTE;

        EK_ASSERT(false, "Wrong shader stage");
        return ShaderStage::NONE;
    }
    String ShaderStageToString(ShaderStage stage)
    {
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
        switch (stage)
        {
        case ShaderStage::VERTEX:   return shaderc_glsl_vertex_shader;
        case ShaderStage::FRAGMENT: return shaderc_glsl_fragment_shader;
        case ShaderStage::COMPUTE:  return shaderc_glsl_compute_shader;
        }
        EK_ASSERT(false, "Unkown shader stage!");
        return 0;
    }

    Shader::Shader(const Path& filePath, const AssetHandle handle) : m_isValid(false)
    {
        Handle = handle;
        m_name = filePath.stem().string();
    }
    Ref<Shader> Shader::Create(const Path& filePath, const AssetHandle handle)
    {
        switch (Renderer::GetAPI())
        {
        case ApiType::Vulkan: return CreateRef<Vulkan::VKShader>(filePath, handle);
        case ApiType::OpenGL: return CreateRef<OpenGL::GLShader>(filePath, handle);
        }
        EK_ASSERT(false, "Shader creation not implemented for current graphics API");
        return nullptr;
    }

    StageSourceMap Shader::Setup()
    {
        EK_PROFILE();
        EK_CORE_TRACE("Setting up shader '{0}'", m_name);

        CreateCacheDirectoryIfNeeded(GetCacheDirectoryPath());

        auto& shaderPath = AssetManager::GetMetadata(Handle).FilePath;
        String source = FileUtilities::ReadFileFromPath(shaderPath);
        m_name = shaderPath.stem().string();

        EK_CORE_DBG("Set up shader '{0}'", m_name);
        return PreProcess(source);
    }
    bool Shader::Recompile()
    {
        EK_PROFILE();
        EK_CORE_TRACE("Recompiling shader '{0}'", m_name);

        Dispose();
        m_isValid = Compile(true);

        // reload all materials that use this shader
        for (auto&& [handle, asset] : AssetManager::GetLoadedAssets())
        {
            if (asset->GetType() != AssetType::Material)
                continue;

            Ref<Material> material = AssetManager::GetAsset<Material>(handle);
            if (material->GetShader().get() == this)
            {
                EK_CORE_TRACE("Reloading material {0}, because shader {1} has been recompiled", material->Handle, Handle);
                material->OnShaderReloaded();
                material->ApplyChanges();
            }
        }

        EK_CORE_DBG("Recompiled shader '{0}'", m_name);

        return m_isValid;
    }

    const String& Shader::GetName() const
    {
        return m_name;
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
        EK_PROFILE();
        EK_CORE_DBG("Shader::Reflect - {0}", shaderName);
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
                const auto& type = compiler.get_type(resource.base_type_id);
                uint32_t location = compiler.get_decoration(resource.id, spv::DecorationLocation);
                if (location > reflection.maxLocation)
                    reflection.maxLocation = location;
                size_t size = GetSizeOfSpirvType(type, name);
                EK_CORE_TRACE("\tName: {0}", name);
                EK_CORE_TRACE("\tLocation: {0}", location);
                EK_CORE_TRACE("\tSize: {0}", size);
                EK_CORE_TRACE("\tOffset: {0}", offset);
                reflection.inputs.push_back({ name, location, size, offset });
                offset += size;
            }
            EK_CORE_TRACE("Outputs:");
            for (const auto& resource : resources.stage_outputs)
            {
                const auto& name = compiler.get_name(resource.id);
                const auto& type = compiler.get_type(resource.base_type_id);
                uint32_t location = compiler.get_decoration(resource.id, spv::DecorationLocation);
                if (location > reflection.maxLocation)
                    reflection.maxLocation = location;
                size_t size = GetSizeOfSpirvType(type, name);
                EK_CORE_TRACE("\tName: {0}", name);
                EK_CORE_TRACE("\tLocation: {0}", location);
                EK_CORE_TRACE("\tSize: {0}", size);
                reflection.outputs.push_back({ name, location, size });
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
                    EK_CORE_TRACE("\t\tType: {0}", DataTypeToString(type));
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
                ShaderPushConstant pushConstant = { name, bufferSize };
                for (size_t memberIndex = 0; memberIndex < bufferType.member_types.size(); ++memberIndex)
                {
                    const auto& name = compiler.get_member_name(bufferType.self, memberIndex);
                    const auto& memberType = compiler.get_type(bufferType.member_types[memberIndex]);
                    size_t memberSize = compiler.get_declared_struct_member_size(bufferType, memberIndex);
                    uint32_t memberOffset = compiler.get_member_decoration(bufferType.self, memberIndex, spv::DecorationOffset);
                    uint32_t memberBinding = compiler.get_member_decoration(bufferType.self, memberIndex, spv::DecorationBinding);
                    ShaderDataType type = SpirvTypeToDataType(memberType);
                    EK_CORE_TRACE("\t\tName: {0}", name);
                    EK_CORE_TRACE("\t\tSize: {0}", memberSize);
                    EK_CORE_TRACE("\t\tOffset: {0}", memberOffset);
                    EK_CORE_TRACE("\t\tType: {0}", DataTypeToString(type));
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
            m_reflections[stage] = reflection;
        }
    }
    bool Shader::CompileOrGetVulkanBinaries(const StageSourceMap& shaderSources, bool forceCompile)
    {
        EK_PROFILE();
        EK_CORE_TRACE("Compiling vulkan binaries for shader '{0}'", m_name);

        bool success = true;
        shaderc::Compiler compiler;
        shaderc::CompileOptions options;
        options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);
        options.SetAutoMapLocations(true);

        //! Strips reflection info
        //options.SetOptimizationLevel(shaderc_optimization_level_performance);

        Path cacheDirectory = "Assets/Cache/Shader/Vulkan";
        CreateCacheDirectoryIfNeeded(cacheDirectory.string());

        auto& shaderPath = AssetManager::GetMetadata(Handle).FilePath;

        auto& shaderData = m_vulkanSPIRV;
        shaderData.clear();
        for (auto&& [stage, source] : shaderSources)
        {
            Path cachedPath = cacheDirectory / (m_name + VKShaderStageCachedVulkanFileExtension(stage));

            std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
            if (!forceCompile && in.is_open())
            {
                EK_CORE_TRACE("Reading Vulkan shader cache binaries from path: '{0}'", cachedPath.string());

                in.seekg(0, std::ios::end);
                auto size = in.tellg();
                in.seekg(0, std::ios::beg);

                auto& data = shaderData[stage];
                data.resize(size / sizeof(uint32_t));
                in.read((char*)data.data(), size);
            }
            else
            {
                EK_CORE_TRACE("Compiling shader {0} to Vulkan binaries", Handle);

                shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, (shaderc_shader_kind)ShaderStageToShaderC(stage), shaderPath.string().c_str(), options);
                if (module.GetCompilationStatus() != shaderc_compilation_status_success)
                {
                    success = false;
                    EK_CORE_ERROR("Failed to compile shader {0}. {1}", Handle, module.GetErrorMessage());
                }
                else
                {
                    shaderData[stage] = Vec<uint32_t>(module.cbegin(), module.cend());

                    EK_CORE_TRACE("Writing Vulkan shader cache binaries to path: '{0}'", cachedPath.string());
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
                        EK_CORE_ERROR("Failed to write Vulkan shader cache binaries to path: '{0}'", cachedPath.string());
                    }
                }
            }
        }

        if (success)
            Reflect(m_vulkanSPIRV, m_name);

        EK_CORE_DBG("Compiled vulkan binaries for shader '{0}'", m_name);
        return success;
    }
    StageSourceMap Shader::PreProcess(const String& source) const
    {
        EK_PROFILE();
        EK_CORE_TRACE("Preprocessing shader source in shader '{0}'", m_name);

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

        EK_CORE_DBG("Preprocessed shader source in shader '{0}'", m_name);

        return shaderSources;
    }
}