#pragma once
#include <Eklipse/Renderer/Material.h>
#include "GLShader.h"

namespace Eklipse
{
	namespace OpenGL
	{
		class GLMaterial : public Eklipse::Material
		{
		public:
			GLMaterial(const std::filesystem::path& path);
			virtual ~GLMaterial() = default;

			virtual void Bind() override;
			virtual void Dispose() override;

		private:
			Ref<GLShader> m_glShader;
		};
	}
}