#pragma once

#include <glm/glm.hpp>

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
		uint32_t binding;
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
		static Ref<Shader> Create(const std::string& filePath);
		Shader() = delete;
		Shader(const std::string& filePath);
		virtual ~Shader() = default;

		std::unordered_map<ShaderStage, std::string> Shader::Setup();
		const std::string& GetName() const;
		const std::unordered_map<ShaderStage, ShaderReflection>& GetReflections() const;
		const ShaderReflection& GetVertexReflection() { return m_reflections[ShaderStage::VERTEX]; }
		const ShaderReflection& GetFragmentReflection() { return m_reflections[ShaderStage::FRAGMENT]; }

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual void Dispose() const = 0;

		inline const std::string& GetPath() const { return m_filePath; }

	protected:
		virtual const std::string GetCacheDirectoryPath() = 0;
		void CompileOrGetVulkanBinaries(const std::unordered_map<ShaderStage, std::string>& shaderSources);
		std::unordered_map<ShaderStage, std::string> PreProcess(const std::string& source);
		void Reflect(const std::unordered_map<ShaderStage, std::vector<uint32_t>>& shaderData, const std::string& shaderName);

	protected:
		std::string m_name;
		std::string m_filePath;
		std::unordered_map<ShaderStage, ShaderReflection> m_reflections;

		std::unordered_map<ShaderStage, std::vector<uint32_t>> m_vulkanSPIRV;

		static std::unordered_map<std::string, std::string> s_shaderResources;
	};

	//class ShaderLibrary
	//{
	//public:
	//	static void Add(const Ref<Shader>& shader);
	//	static Ref<Shader> Load(const std::string& filePath);
	//	static Ref<Shader> Get(const std::string& name);
	//	static bool Contains(const std::string& name);
	//	static void Dispose();

	//private:
	//	static std::unordered_map<std::string, Ref<Shader>> m_shaders;
	//};
}