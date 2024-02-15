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
			GLMaterial(const Ref<Shader> shader);

			virtual void Bind() override;
			virtual void Dispose() override;

			virtual void SetShader(AssetHandle shaderHandle) override;
			virtual void SetShader(const Ref<Shader> shader) override;

		private:
			Ref<GLShader> m_glShader;
		};
	}
}