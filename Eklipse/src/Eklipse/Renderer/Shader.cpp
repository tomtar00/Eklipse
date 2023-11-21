#include "precompiled.h"
#include "Shader.h"

#include <Eklipse/Renderer/Renderer.h>
#include <Eklipse/Platform/OpenGL/GLShader.h>
#include <Eklipse/Platform/Vulkan/VKShader.h>
#include <Eklipse/Utils/File.h>

#include <filesystem>
#include <fstream>
#include <shaderc/shaderc.hpp>
#include <spirv_cross.hpp>
#include <spirv_glsl.hpp>

namespace Eklipse
{
    ShaderStage StringToShaderStage(const std::string& stage)
    {
        if (stage == "vertex")   return ShaderStage::VERTEX;
        if (stage == "fragment") return ShaderStage::FRAGMENT;
        if (stage == "compute")  return ShaderStage::COMPUTE;

        EK_ASSERT(false, "Wrong shader stage");
        return ShaderStage::NONE;
    }
    std::string ShaderStageToString(ShaderStage stage)
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
    static void CreateCacheDirectoryIfNeeded(const std::string& cacheDirectory)
    {
        if (!std::filesystem::exists(cacheDirectory))
            std::filesystem::create_directories(cacheDirectory);
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
    static size_t GetSizeOfSPIRVType(const spirv_cross::SPIRType type, const std::string& name)
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
    static DataType SPIRVTypeToDataType(const spirv_cross::SPIRType type)
    {
        if (type.basetype == spirv_cross::SPIRType::Image && type.image.dim == spv::Dim2D) return DataType::SAMPLER2D;
        if (type.columns == 3 && type.vecsize == 3) return DataType::MAT3;
        if (type.columns == 4 && type.vecsize == 4) return DataType::MAT4;

        if (type.basetype == spirv_cross::SPIRType::Boolean) return DataType::BOOL;
        if (type.basetype == spirv_cross::SPIRType::Float)
        {
			if (type.vecsize == 1) return DataType::FLOAT;
			if (type.vecsize == 2) return DataType::FLOAT2;
			if (type.vecsize == 3) return DataType::FLOAT3;
			if (type.vecsize == 4) return DataType::FLOAT4;
		}
        if (type.basetype == spirv_cross::SPIRType::Int || type.basetype == spirv_cross::SPIRType::UInt)
        {
            if (type.vecsize == 1) return DataType::INT;
            if (type.vecsize == 2) return DataType::INT2;
            if (type.vecsize == 3) return DataType::INT3;
            if (type.vecsize == 4) return DataType::INT4;
        }
        
        EK_ASSERT(false, "Unkown SPIR-V type! Type: {0}", (uint32_t)type.basetype);
        return DataType::NONE;
    }
    static std::string DataTypeToString(DataType type)
    {
        switch (type)
        {
			case DataType::NONE:     return "none";
			case DataType::BOOL:     return "bool";
			case DataType::INT:      return "int";
			case DataType::INT2:     return "int2";
			case DataType::INT3:     return "int3";
			case DataType::INT4:     return "int4";
			case DataType::FLOAT:    return "float";
			case DataType::FLOAT2:   return "float2";
			case DataType::FLOAT3:   return "float3";
			case DataType::FLOAT4:   return "float4";
			case DataType::MAT3:     return "mat3";
			case DataType::MAT4:     return "mat4";
			case DataType::SAMPLER2D:return "sampler2D";
		}
		EK_ASSERT(false, "Unkown data type!");
		return "none";
	}

    std::unordered_map<ShaderStage, std::string> Shader::PreProcess(const std::string& source)
    {
        std::unordered_map<ShaderStage, std::string> shaderSources;

        const char* typeToken = "#stage";
        size_t typeTokenLength = strlen(typeToken);
        size_t pos = source.find(typeToken, 0);
        while (pos != std::string::npos)
        {
            size_t eol = source.find_first_of("\r\n", pos);
            EK_ASSERT(eol != std::string::npos, "Shader stage syntax error (1)");
            size_t begin = pos + typeTokenLength + 1;
            std::string type = source.substr(begin, eol - begin);
            EK_ASSERT(StringToShaderStage(type) != ShaderStage::NONE, "Invalid shader stage specified");

            size_t nextLinePos = source.find_first_not_of("\r\n", eol);
            EK_ASSERT(nextLinePos != std::string::npos, "Shader stage syntax error (2)");
            pos = source.find(typeToken, nextLinePos);

            shaderSources[StringToShaderStage(type)] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
        }

        return shaderSources;
    }
    bool Shader::CompileOrGetVulkanBinaries(const std::unordered_map<ShaderStage, std::string>& shaderSources, bool forceCompile)
    {
        bool success = true;
        shaderc::Compiler compiler;
        shaderc::CompileOptions options;
        options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);
        options.SetAutoMapLocations(true);

        //! Strips reflection info
        //options.SetOptimizationLevel(shaderc_optimization_level_performance);

        std::filesystem::path cacheDirectory = "Assets/Cache/Shader/Vulkan";
        CreateCacheDirectoryIfNeeded(cacheDirectory.string());

        auto& shaderData = m_vulkanSPIRV;
        shaderData.clear();
        for (auto&& [stage, source] : shaderSources)
        {
            std::filesystem::path shaderFilePath = m_filePath;
            std::filesystem::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() + VKShaderStageCachedVulkanFileExtension(stage));

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
                EK_CORE_TRACE("Compiling shader at path: '{0}' to Vulkan binaries", m_filePath);

                shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, (shaderc_shader_kind)ShaderStageToShaderC(stage), m_filePath.c_str(), options);
                if (module.GetCompilationStatus() != shaderc_compilation_status_success)
                {
                    success = false;
                    EK_CORE_ERROR("Failed to compile shader at path: '{0}'\n\t{1}", m_filePath, module.GetErrorMessage());
                }
                else
                {
                    shaderData[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());

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
            Reflect(m_vulkanSPIRV, m_filePath);
        return success;
    }
    void Shader::Reflect(const std::unordered_map<ShaderStage, std::vector<uint32_t>>& shaderStagesData, const std::string& shaderName)
    {
        EK_CORE_WARN("Shader::Reflect - {0}", shaderName);
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
                size_t size = GetSizeOfSPIRVType(type, name);
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
                size_t size = GetSizeOfSPIRVType(type, name);
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
                    DataType type = SPIRVTypeToDataType(memberType);
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
                    DataType type = SPIRVTypeToDataType(memberType);
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

    Ref<Shader> Shader::Create(const Path& filePath)
    {
        auto apiType = Renderer::GetAPI();
        switch (apiType)
        {
            case ApiType::Vulkan: return CreateRef<Vulkan::VKShader>(filePath);
            case ApiType::OpenGL: return CreateRef<OpenGL::GLShader>(filePath);
        }
        EK_ASSERT(false, "API {0} not implemented for Shader creation", int(apiType));
        return nullptr;
    }
    Shader::Shader(const Path& filePath) : m_filePath(filePath), m_isValid(false)
    {
        m_name = filePath.path().stem().string();
    }
    std::unordered_map<ShaderStage, std::string> Shader::Setup()
    {
        CreateCacheDirectoryIfNeeded(GetCacheDirectoryPath());

        std::string source = ReadFileFromPath(m_filePath);

        auto lastSlash = m_filePath.string().find_last_of("/\\");
        lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
        auto lastDot = m_filePath.string().rfind('.');
        auto count = lastDot == std::string::npos ? m_filePath.size() - lastSlash : lastDot - lastSlash;
        m_name = m_filePath.string().substr(lastSlash, count);

        return PreProcess(source);
    }
    const std::string& Shader::GetName() const
    {
        return m_name;
    }

    const std::unordered_map<ShaderStage, ShaderReflection>& Shader::GetReflections() const
    {
        return m_reflections;
    }

    //////////////////////////////////////////////////////////////////
    // SHADER LIBRARY ////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////

   /* void ShaderLibrary::Add(const Ref<Shader>& shader)
    {
        if (Contains(shader->GetName()))
        {
            EK_CORE_WARN("Shader library already contains shader '{0}'", shader->GetName());
            return;
        }

        m_shaders[shader->GetName()] = shader;
        EK_CORE_INFO("Adding shader '{0}' to library", shader->GetName());
    }
    Ref<Shader> ShaderLibrary::Load(const std::string& filePath)
    {
        auto shader = Shader::Create(filePath);
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
    }*/
}