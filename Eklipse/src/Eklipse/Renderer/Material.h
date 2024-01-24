#pragma once
#include <Eklipse/Renderer/Shader.h>
#include <Eklipse/Renderer/Texture.h>

namespace Eklipse
{
	struct PushConstantData
	{
		void* data;
		size_t size;
		ShaderDataType type;
	};

	using PushConstantDataMap = std::unordered_map<std::string, PushConstantData>;

	struct PushConstant
	{
		PushConstant() = default;

		PushConstant(const PushConstant& other);
		PushConstant& operator=(const PushConstant& other);
		void Copy(const PushConstant& other);

		PushConstantDataMap dataPointers;
		Unique<char[]> pushConstantData;
		size_t pushConstantSize;
	};
	struct Sampler2D
	{
		uint32_t binding;
		AssetHandle textureHandle;
		Ref<Texture2D> texture;
	};

	using PushConstantMap = std::map<std::string, PushConstant>;
	using Sampler2DMap = std::map<std::string, Sampler2D>;

	class Material : public Asset
	{
	public:
		Material() = delete;
		Material(const Path& path);
		Material(const Path& path, AssetHandle shaderHandle);
		static Ref<Material> Create(const Path& path);
		static Ref<Material> Create(const Path& path, AssetHandle shaderHandle);

		template <typename T>
		void SetConstant(const std::string& constantName, const std::string& memberName, const T* data, size_t size);

		virtual void Bind();
		virtual void ApplyChanges();

		void SetShader(Ref<Shader> shader);
		void OnShaderReloaded();
		bool Serialize(const Path& path);
		bool Deserialize(const Path& path);

		const std::string& GetName() const;
		const Ref<Shader> GetShader() const;
		const PushConstantMap& GetPushConstants() const;
		const Sampler2DMap& GetSamplers() const;
		bool IsValid() const;

		static AssetType GetStaticType() { return AssetType::Material; }
		virtual AssetType GetType() const override { return GetStaticType(); }

		virtual void Dispose() = 0;

	protected:
		std::string m_name;
		PushConstantMap m_pushConstants{};
		Sampler2DMap m_samplers{};

		Ref<Shader> m_shader;
	};

	template <typename T>
	inline void Material::SetConstant(const std::string& constantName, const std::string& memberName, const T* data, size_t size)
	{
		EK_PROFILE();

		EK_ASSERT(m_pushConstants.find(constantName) != m_pushConstants.end(), "({0}) Push constant '{1}' not found", m_name, constantName);
		auto& pushConstant = m_pushConstants.at(constantName);
		EK_ASSERT(pushConstant.dataPointers.find(memberName) != pushConstant.dataPointers.end(), "({0}) Push constant '{1}' member '{2}' not found", m_name, constantName, memberName);

		auto& dataPointer = pushConstant.dataPointers.at(memberName);
		EK_ASSERT(dataPointer.size == size, "({0}) Push constant '{1}' member '{2}' size mismatch. Required = {3} Given = {4}", m_name, constantName, memberName, dataPointer.size, size);

		std::memcpy(dataPointer.data, data, size);
	}
}