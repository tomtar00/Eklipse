#pragma once
#include <vulkan/vulkan.h>
#include <Eklipse/Renderer/Material.h>
#include "VKBuffers.h"
#include "VKShader.h"
#include "VKTexture.h"

namespace Eklipse
{
	namespace Vulkan
	{
		class VKMaterial : public Eklipse::Material
		{
		public:
			VKMaterial(const Path& path, AssetHandle shaderHandle);
			VKMaterial(const Ref<Shader> shader);

			virtual void Bind() override;
			virtual void Dispose() override;
			virtual void ApplyChanges(const Path& filePath) override;

			virtual void SetShader(AssetHandle shaderHandle) override;
			virtual void SetShader(const Ref<Shader> shader) override;

			virtual void SetSampler(const String& samplerName, const Ref<Texture2D> texture) override;

		private:
			void CreateDescriptorSets();

		private:
			Ref<VKShader> m_vkShader;
			Vec<VkDescriptorSet> m_descriptorSets;
		};
	}
}