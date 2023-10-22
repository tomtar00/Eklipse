#pragma once
#include <Eklipse/Renderer/Shader.h>

namespace Eklipse
{
	struct PushConstantData
	{
		void* data;
		size_t size;
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
		Material(Ref<Shader> shader);

		virtual void SetConstant(const std::string& constantName, const std::string& memberName, const void* data, size_t size);

		virtual void Bind();
		virtual void Dispose();

		static Ref<Material> Create(Ref<Shader> shader);

	protected:
		Ref<Shader> m_shader;
		std::unordered_map<std::string, PushConstant> m_pushConstants;
	};
}