#pragma once
#include <Eklipse/Renderer/Shader.h>

namespace Eklipse
{
	class Material
	{
	public:
		Material() = default;
		Material(Ref<Shader> shader) : m_shader(shader) {}

		virtual void Bind() = 0;

		static Ref<Material> Create(Ref<Shader> shader);

	protected:
		Ref<Shader> m_shader;
	};
}