#pragma once
#include <Eklipse/Renderer/Material.h>
#include "GLShader.h"

namespace Eklipse
{
	namespace OpenGL
	{
		class GLMaterial : public Material
		{
		public:
			GLMaterial(const Path& path, AssetHandle shaderHandle);

			virtual void Bind() override;
			virtual void Dispose() const override;

		private:
			Ref<GLShader> m_glShader;
		};
	}
}