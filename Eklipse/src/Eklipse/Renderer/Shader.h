#pragma once

#include <glm/glm.hpp>
#include <Eklipse/Utils/File.h>

namespace Eklipse
{
	enum class ShaderStage
	{
		NONE,
		VERTEX,
		FRAGMENT,
		COMPUTE
	};
	enum class DataType
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
		DataType type;
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

	extern ShaderStage StringToShaderStage(const std::string& stage);
	extern std::string ShaderStageToString(ShaderStage stage);
	extern uint32_t ShaderStageToShaderC(const ShaderStage stage);
	
	class Shader
	{
	public:
		static Ref<Shader> Create(const Path& filePath);
		Shader() = delete;
		Shader(const Path& filePath);
		virtual ~Shader() = default;

		std::unordered_map<ShaderStage, std::string> Shader::Setup();
		const std::string& GetName() const;
		const std::unordered_map<ShaderStage, ShaderReflection>& GetReflections() const;
		const ShaderReflection& GetVertexReflection() { return m_reflections[ShaderStage::VERTEX]; }
		const ShaderReflection& GetFragmentReflection() { return m_reflections[ShaderStage::FRAGMENT]; }

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual void Dispose() const = 0;

		inline bool Recompile() { Dispose(); return m_isValid = Compile(true); }
		inline bool IsValid() const { return m_isValid; }

		inline Path& GetPath() { return m_filePath; }

	protected:
		virtual bool Compile(bool forceCompile = false) = 0;
		virtual const std::string GetCacheDirectoryPath() = 0;
		bool CompileOrGetVulkanBinaries(const std::unordered_map<ShaderStage, std::string>& shaderSources, bool forceCompile);
		std::unordered_map<ShaderStage, std::string> PreProcess(const std::string& source);
		void Reflect(const std::unordered_map<ShaderStage, std::vector<uint32_t>>& shaderData, const std::string& shaderName);

	protected:
		std::string m_name;
		Path m_filePath;
		bool m_isValid;

		std::unordered_map<ShaderStage, ShaderReflection> m_reflections;
		std::unordered_map<ShaderStage, std::vector<uint32_t>> m_vulkanSPIRV;
	};
}