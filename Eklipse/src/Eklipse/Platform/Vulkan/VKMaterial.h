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
			VKMaterial(Ref<Shader> shader);
			virtual ~VKMaterial() = default;

			virtual void Bind() override;
			virtual void Dispose() override;

		private:
			void CreateDescriptorSets();

		private:
			Ref<VKShader> m_vkShader;
			std::vector<VkDescriptorSet> m_descriptorSets;
		};
	}
}