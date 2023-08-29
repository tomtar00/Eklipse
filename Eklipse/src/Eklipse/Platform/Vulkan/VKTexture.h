#pragma once

#include <Eklipse/Renderer/Texture.h>

namespace Eklipse
{
	namespace Vulkan
	{
		class VKTexture2D : public Eklipse::Texture2D
		{
		public:
			VKTexture2D(const std::string& texturePath);
			VKTexture2D(const TextureInfo& textureInfo);
			virtual ~VKTexture2D();

			virtual void SetData(void* data, uint32_t size) override;
			virtual void Bind() const override;
		};
	}
}