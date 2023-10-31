#pragma once
#include <Eklipse/Renderer/Shader.h>
#include <Eklipse/Renderer/Texture.h>
#include <filesystem>

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
		std::unordered_map<std::string, PushConstantData> dataPointers;
		Unique<char[]> pushConstantData;
		size_t pushConstantSize;
	};

	class Material
	{
	public:
		Material() = delete;
		Material(const std::filesystem::path& path);

		template <typename T>
		void SetConstant(const std::string& constantName, const std::string& memberName, const T* data, size_t size);
		void SetSampler(const std::string& samplerName, const std::filesystem::path& texturePath);

		virtual void Bind();
		virtual void Dispose();

		void SetShader(Ref<Shader> shader);
		void Serialize(const std::filesystem::path& path);
		void Deserialize(const std::filesystem::path& path);
		inline const std::string& GetName() const { return m_name; }
		inline const std::filesystem::path& GetPath() const { return m_path; }

		static Ref<Material> Create(const std::filesystem::path& path);

	protected:
		Ref<Shader> m_shader;
		std::unordered_map<std::string, PushConstant> m_pushConstants;
		std::unordered_map<std::string, std::filesystem::path> m_samplers;
		std::vector<Ref<Texture2D>> m_sampledTextures;

		std::string m_name;
		std::filesystem::path m_path;
	};

	template <typename T>
	inline void Material::SetConstant(const std::string& constantName, const std::string& memberName, const T* data, size_t size)
	{
		EK_ASSERT(m_pushConstants.find(constantName) != m_pushConstants.end(), "Push constant '{0}' not found", constantName);
		auto& pushConstant = m_pushConstants[constantName];
		EK_ASSERT(pushConstant.dataPointers.find(memberName) != pushConstant.dataPointers.end(), "Push constant '{0}' member '{1}' not found", constantName, memberName);

		auto& dataPointer = pushConstant.dataPointers[memberName];
		EK_ASSERT(dataPointer.size == size, "Push constant '{0}' member '{1}' size mismatch. Required = {2} Given = {3}", constantName, memberName, dataPointer.size, size);

		std::memcpy(dataPointer.data, data, size);
	}
}