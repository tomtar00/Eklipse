#pragma once
#include <Eklipse/Renderer/Shader.h>

namespace Eklipse
{
	class Material
	{
	public:
		Material() = delete;
		Material(Ref<Shader> shader) : m_shader(shader) {}

		virtual void Bind();
		virtual void Dispose();

		static Ref<Material> Create(Ref<Shader> shader);

	protected:
		Ref<Shader> m_shader;
	};
}