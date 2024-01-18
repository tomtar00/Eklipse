#pragma once
#include <Eklipse/Renderer/Shader.h>
#include <Eklipse/Renderer/Texture.h>

namespace Eklipse
{
	struct PushConstantData
	{
		void* data;
		size_t size;
		DataType type;
	};
	struct PushConstant
	{
		PushConstant() = default;

		PushConstant(const PushConstant& other);
		PushConstant& operator=(const PushConstant& other);
		void Copy(const PushConstant& other);

		std::unordered_map<std::string, PushConstantData> dataPointers;
		Unique<char[]> pushConstantData;
		size_t pushConstantSize;
	};
	struct Sampler2D
	{
		uint32_t binding;
		Path texturePath;
		Ref<Texture2D> texture;
	};

	class EK_API Material
	{
	public:
		Material() = delete;
		Material(const Path& path, const Path& shaderPath);

		template <typename T>
		void SetConstant(const std::string& constantName, const std::string& memberName, const T* data, size_t size);

		virtual void Bind();
		virtual void Dispose() = 0;

		virtual void ApplyChanges();

		void SetShader(Ref<Shader> shader);
		void SetShader(const Path& shaderPath);
		void OnShaderReloaded();
		void Serialize(const Path& path);
		void Deserialize(const Path& path);

		inline const std::string& GetName() const { return m_name; }
		inline const Path& GetPath() const { return m_path; }
		inline const Ref<Shader> GetShader() const { return m_shader; }
		inline const std::unordered_map<std::string, PushConstant>& GetPushConstants() const { return m_pushConstants; }
		inline std::unordered_map<std::string, Sampler2D>& GetSamplers() { return m_samplers; }
		inline bool IsValid() const { return m_shader != nullptr && m_shader->IsValid(); }

		static Ref<Material> Create(const Path& path, const Path& shaderPath);

	protected:
		Ref<Shader> m_shader;
		std::unordered_map<std::string, PushConstant> m_pushConstants{};
		std::unordered_map<std::string, Sampler2D> m_samplers{};

		std::string m_name;
		Path m_path;
	};

	template <typename T>
	inline void Material::SetConstant(const std::string& constantName, const std::string& memberName, const T* data, size_t size)
	{
		EK_PROFILE();

		EK_ASSERT(m_pushConstants.find(constantName) != m_pushConstants.end(), "({0}) Push constant '{1}' not found", m_name, constantName);
		auto& pushConstant = m_pushConstants[constantName];
		EK_ASSERT(pushConstant.dataPointers.find(memberName) != pushConstant.dataPointers.end(), "({0}) Push constant '{1}' member '{2}' not found", m_name, constantName, memberName);

		auto& dataPointer = pushConstant.dataPointers[memberName];
		EK_ASSERT(dataPointer.size == size, "({0}) Push constant '{1}' member '{2}' size mismatch. Required = {3} Given = {4}", m_name, constantName, memberName, dataPointer.size, size);

		std::memcpy(dataPointer.data, data, size);
	}
}