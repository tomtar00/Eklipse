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
			VKMaterial(const Path& path);
			VKMaterial(const Path& path, AssetHandle shaderHandle);

			virtual void Bind() override;
			virtual void Dispose() override;
			virtual void ApplyChanges() override;

		private:
			void CreateDescriptorSets();

		private:
			Ref<VKShader> m_vkShader;
			std::vector<VkDescriptorSet> m_descriptorSets;
		};
	}
}