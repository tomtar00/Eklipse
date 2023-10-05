#pragma once

#include <glm/glm.hpp>

namespace Eklipse
{
	class Shader
	{
	public:
		static Ref<Shader> Create(const std::string& filePath);
		virtual ~Shader() = default;

		const std::string& GetName() const;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual void Dispose() const = 0;

		virtual void UploadMat4(const std::string& name, const glm::mat4& matrix) = 0;
		virtual void UploadInt(const std::string& name, const int value) = 0;

	protected:
		std::string m_name;
	};

	class ShaderLibrary
	{
	public:
		void Add(const Ref<Shader>& shader);
		Ref<Shader> Load(const std::string& filePath);
		Ref<Shader> Get(const std::string& name);
		bool Contains(const std::string& name);
		void Dispose();

	private:
		std::unordered_map<std::string, Ref<Shader>> m_shaders;
	};
}