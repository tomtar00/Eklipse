#pragma once

#include <glm/glm.hpp>
#include <Eklipse/Utils/File.h>
#include <Eklipse/Assets/Asset.h>

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
		FLOAT, FLOAT2, FLOAT3, FLOAT4, 
		MAT3, MAT4, 
		SAMPLER2D
	};

	// Buffer data
	struct ShaderUniformMember
	{
		std::string name;
		size_t size;
		size_t offset;
		ShaderDataType type;
	};
	struct ShaderUniformBuffer
	{
		std::string name;
		size_t size;
		uint32_t binding;
		std::vector<ShaderUniformMember> members;
	};
	struct ShaderPushConstant
	{
		std::string name;
		size_t size;
		std::vector<ShaderUniformMember> members;
	};

	// Samplers
	struct ShaderSampler
	{
		std::string name;
		uint32_t binding;
	};
	
	// Inputs and outputs
	struct ShaderInput
	{
		std::string name;
		uint32_t location;
		size_t size;
		size_t offset;
	};
	struct ShaderOutput
	{
		std::string name;
		uint32_t location;
		size_t size;
	};
	
	// Reflection
	struct ShaderReflection
	{
		std::vector<ShaderInput> inputs;
		std::vector<ShaderOutput> outputs;
		std::vector<ShaderUniformBuffer> uniformBuffers;
		std::vector<ShaderPushConstant> pushConstants;
		std::vector<ShaderSampler> samplers;
		uint32_t maxLocation = 0;
	};

	ShaderStage StringToShaderStage(const std::string& stage);
	std::string ShaderStageToString(ShaderStage stage);
	uint32_t ShaderStageToShaderC(const ShaderStage stage);

	using StageReflectionMap = std::map<ShaderStage, ShaderReflection>;
	using StageSourceMap = std::unordered_map<ShaderStage, std::string>;
	using StageSpirvMap = std::unordered_map<ShaderStage, std::vector<uint32_t>>;
	
	class Shader : public Asset
	{
	public:
		Shader() = delete;
		Shader(const Path& filePath);
		static Ref<Shader> Create(const Path& filePath);

		StageSourceMap Shader::Setup();
		bool Recompile();

		const std::string& GetName() const;
		const StageReflectionMap& GetReflections() const;
		const ShaderReflection& GetVertexReflection();
		const ShaderReflection& GetFragmentReflection();
		bool IsValid() const;

		static AssetType GetStaticType() { return AssetType::Shader; }
		virtual AssetType GetType() const override { return GetStaticType(); }

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual void Dispose() const = 0;

	protected:
		void Reflect(const StageSpirvMap& shaderData, const std::string& shaderName);
		bool CompileOrGetVulkanBinaries(const StageSourceMap& shaderSources, bool forceCompile);
		StageSourceMap PreProcess(const std::string& source) const;

		virtual bool Compile(bool forceCompile = false) = 0;
		virtual const std::string GetCacheDirectoryPath() = 0;

	protected:
		std::string m_name;
		bool m_isValid;

		StageReflectionMap m_reflections;
		StageSpirvMap m_vulkanSPIRV;
	};
}