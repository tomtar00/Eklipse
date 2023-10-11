#pragma once
#include <Eklipse/Renderer/Material.h>

namespace Eklipse
{
	namespace OpenGL
	{
		class GLMaterial : public Eklipse::Material
		{
		public:
			GLMaterial(Ref<Shader> shader);
			~GLMaterial();

			virtual void Bind() override;
		};
	}
}