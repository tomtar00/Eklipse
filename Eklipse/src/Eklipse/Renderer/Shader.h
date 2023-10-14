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

	struct ShaderUniform
	{
		std::string name;
		size_t size;
		size_t offset;
		uint32_t binding;
	};
	struct ShaderSampler
	{
		std::string name;
		uint32_t binding;
	};
	struct ShaderUniformBuffer
	{
		std::string name;
		size_t size;
		uint32_t binding;
		std::vector<ShaderUniform> uniforms;
	};
	struct ShaderOutput
	{
		std::string name;
		uint32_t location;
		size_t size;
	};
	struct ShaderInput
	{
		std::string name;
		uint32_t location;
		size_t size;
		size_t offset;
	};
	struct ShaderReflection
	{
		std::vector<ShaderInput> inputs;
		std::vector<ShaderOutput> outputs;
		std::vector<ShaderUniformBuffer> uniformBuffers;
		std::vector<ShaderSampler> samplers;
	};

	extern ShaderStage StringToShaderStage(const std::string& stage);
	extern std::string ShaderStageToString(ShaderStage stage);
	extern uint32_t ShaderStageToShaderC(const ShaderStage stage);
	extern void CreateCacheDirectoryIfNeeded(std::string cacheDirectory);
	
	class Shader
	{
	public:
		static Ref<Shader> Create(const std::string& filePath);
		virtual ~Shader() = default;

		const std::string& GetName() const;
		const std::unordered_map<ShaderStage, ShaderReflection>& GetReflections() const;
		const ShaderReflection& GetVertexReflection() { return m_reflections[ShaderStage::VERTEX]; }
		const ShaderReflection& GetFragmentReflection() { return m_reflections[ShaderStage::FRAGMENT]; }

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual void Dispose() const = 0;

	protected:
		std::unordered_map<ShaderStage, std::string> PreProcess(const std::string& source);
		void Reflect(const std::unordered_map<ShaderStage, std::vector<uint32_t>>& shaderData, const std::string& shaderName);

	protected:
		std::string m_name;
		std::unordered_map<ShaderStage, ShaderReflection> m_reflections;

		static std::unordered_map<std::string, std::string> s_shaderResources;
	};

	class ShaderLibrary
	{
	public:
		static void Add(const Ref<Shader>& shader);
		static Ref<Shader> Load(const std::string& filePath);
		static Ref<Shader> Get(const std::string& name);
		static bool Contains(const std::string& name);
		static void Dispose();

	private:
		static std::unordered_map<std::string, Ref<Shader>> m_shaders;
	};
}