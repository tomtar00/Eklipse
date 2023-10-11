#include "precompiled.h"
#include "Shader.h"

#include <Eklipse/Renderer/Renderer.h>
#include <Eklipse/Platform/OpenGL/GLShader.h>
#include <Eklipse/Platform/Vulkan/VKShader.h>

#include <filesystem>
#include <fstream>
#include <shaderc/shaderc.hpp>
#include <spirv_cross.hpp>
#include <spirv_glsl.hpp>

namespace Eklipse
{
    std::unordered_map<std::string, Ref<Shader>> ShaderLibrary::m_shaders{};

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
    void CreateCacheDirectoryIfNeeded(std::string cacheDirectory)
    {
        if (!std::filesystem::exists(cacheDirectory))
            std::filesystem::create_directories(cacheDirectory);
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
                const auto& type = compiler.get_type(resource.base_type_id);
                uint32_t location = compiler.get_decoration(resource.id, spv::DecorationLocation);
                size_t size = GetSizeOfSPIRVType(type, resource.name);
                EK_CORE_TRACE("\tName: {0}", resource.name);
                EK_CORE_TRACE("\tLocation: {0}", location);
                EK_CORE_TRACE("\tSize: {0}", size);
                EK_CORE_TRACE("\tOffset: {0}", offset);
                reflection.inputs.push_back({ resource.name, location, size, offset });
                offset += size;
            }
            EK_CORE_TRACE("Outputs:");
            for (const auto& resource : resources.stage_outputs)
            {
                const auto& type = compiler.get_type(resource.base_type_id);
                uint32_t location = compiler.get_decoration(resource.id, spv::DecorationLocation);
                size_t size = GetSizeOfSPIRVType(type, resource.name);
                EK_CORE_TRACE("\tName: {0}", resource.name);
                EK_CORE_TRACE("\tLocation: {0}", location);
                EK_CORE_TRACE("\tSize: {0}", size);
                reflection.outputs.push_back({ resource.name, location, size });
            }
            EK_CORE_TRACE("Uniform buffers:");
            for (const auto& resource : resources.uniform_buffers)
            {
                const auto& bufferType = compiler.get_type(resource.base_type_id);
                size_t bufferSize = compiler.get_declared_struct_size(bufferType);
                uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
                EK_CORE_TRACE("\tName: {0}", resource.name);
                EK_CORE_TRACE("\tSize: {0}", bufferSize);
                EK_CORE_TRACE("\tBinding: {0}", binding);
                ShaderUniformBuffer uniformBuffer = { resource.name, bufferSize, binding };
                for (size_t memberIndex = 0; memberIndex < bufferType.member_types.size(); ++memberIndex)
                {
                    const auto& memberType = compiler.get_type(bufferType.member_types[memberIndex]);
                    size_t memberSize = compiler.get_declared_struct_member_size(bufferType, memberIndex);
                    uint32_t memberOffset = compiler.get_member_decoration(bufferType.self, memberIndex, spv::DecorationOffset);
                    uint32_t memberBinding = compiler.get_member_decoration(bufferType.self, memberIndex, spv::DecorationBinding);
                    EK_CORE_TRACE("\t\tName: {0}", compiler.get_member_name(bufferType.self, memberIndex));
                    EK_CORE_TRACE("\t\tSize: {0}", memberSize);
                    EK_CORE_TRACE("\t\tOffset: {0}", memberOffset);
                    EK_CORE_TRACE("\t\tBinding: {0}", memberBinding);
                    uniformBuffer.uniforms.push_back({ compiler.get_member_name(bufferType.self, memberIndex), memberSize, memberOffset, memberBinding });
                }
                reflection.uniformBuffers.push_back(uniformBuffer);
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


    Ref<Shader> Shader::Create(const std::string& filePath)
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
    }
}