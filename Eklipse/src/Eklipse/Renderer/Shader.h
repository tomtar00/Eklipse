#pragma once

#include <glm/glm.hpp>
#include <Eklipse/Utils/File.h>
#include <Eklipse/Assets/Asset.h>

#include <shaderc/shaderc.hpp>

namespace Eklipse
{
	enum class ShaderStage
	{
		NONE,
		VERTEX,
		FRAGMENT,
		COMPUTE
	};
	enum class ShaderDataType
	{
		NONE,
		BOOL, 
		INT, INT2, INT3, INT4,
		UINT, UINT2, UINT3, UINT4,
		FLOAT, FLOAT2, FLOAT3, FLOAT4, 
		MAT3, MAT4, 
		SAMPLER2D,
		STRUCT
	};

	// Buffer data
	struct ShaderUniformMember
	{
		String name;
		size_t size;
		size_t offset;
		ShaderDataType type;
	};
	struct ShaderUniformBuffer
	{
		String name;
		size_t size;
		uint32_t binding;
		Vec<ShaderUniformMember> members;
	};
	struct ShaderPushConstant
	{
		String name;
		size_t size;
		uint32_t offset;
		Vec<ShaderUniformMember> members;
	};
	struct ShaderSampler
	{
		String name;
		uint32_t binding;
	};
	struct ShaderStorageBuffer
	{
        String name;
        size_t size;
        uint32_t binding;
        Vec<ShaderUniformMember> members;
    };
	
	// Inputs and outputs
	struct ShaderInput
	{
		String name;
		uint32_t location;
		size_t size;
		size_t offset;
		ShaderDataType type;
	};
	struct ShaderOutput
	{
		String name;
		uint32_t location;
		size_t size;
		ShaderDataType type;
	};
	
	// Reflection
	struct ShaderReflection
	{
		Vec<ShaderInput> inputs;
		Vec<ShaderOutput> outputs;
		Vec<ShaderUniformBuffer> uniformBuffers;
		Vec<ShaderPushConstant> pushConstants;
		Vec<ShaderSampler> samplers;
		Vec<ShaderStorageBuffer> storageBuffers;
		uint32_t maxLocation = 0;
	};

	EK_API ShaderStage StringToShaderStage(const String& stage);
	EK_API String ShaderStageToString(ShaderStage stage);
	EK_API uint32_t ShaderStageToShaderC(const ShaderStage stage);
	EK_API String ShaderDataTypeToString(ShaderDataType type);

	using StageReflectionMap = std::map<ShaderStage, ShaderReflection>;
	using StageSourceMap = std::unordered_map<ShaderStage, String>;
	using StageSpirvMap = std::unordered_map<ShaderStage, Vec<uint32_t>>;
	
	class EK_API Shader : public Asset
	{
	public:
		Shader() = delete;
		Shader(const String& vertexSource, const String& fragmentSource, const AssetHandle handle = AssetHandle());
		Shader(const Path& filePath, const AssetHandle handle = AssetHandle());
		static Ref<Shader> Create(const Path& filePath, const AssetHandle handle = AssetHandle());
		static Ref<Shader> Create(const String& vertexSource, const String& fragmentSource, const AssetHandle handle = AssetHandle());

		StageSourceMap Shader::Setup(const Path& shaderPath);
		bool Recompile(const Path& shaderPath);

		const StageReflectionMap& GetReflections() const;
		const ShaderReflection& GetVertexReflection();
		const ShaderReflection& GetFragmentReflection();
		bool IsValid() const;

		static AssetType GetStaticType() { return AssetType::Shader; }
		virtual AssetType GetType() const override { return GetStaticType(); }

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual void Dispose() = 0;

		virtual bool Compile(const Path& shaderPath, bool forceCompile = false) = 0;
		virtual bool Compile(const StageSourceMap& sourceMap, bool forceCompile = false) = 0;

	protected:

		void Reflect(const StageSpirvMap& shaderData, const String& shaderName);
		bool CompileOrGetVulkanBinaries(const Path& shaderPath, const StageSourceMap& shaderSources, bool forceCompile);
		StageSourceMap PreProcess(const String& source) const;

		virtual bool Compile(const Path& shaderPath, const StageSourceMap& sourceMap, bool forceCompile = false) = 0;
		virtual const String GetCacheDirectoryPath() = 0;

	protected:
		bool m_isValid;

		StageReflectionMap m_reflections;
		StageSpirvMap m_vulkanSPIRV;
	};

	class ShaderIncluder : public shaderc::CompileOptions::IncluderInterface
	{
	public:
        virtual shaderc_include_result* GetInclude(const char* requested_source, shaderc_include_type type, const char* requesting_source, size_t include_depth) override;
        virtual void ReleaseInclude(shaderc_include_result* data) override;
	};
}