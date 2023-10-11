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
			~VKMaterial();

			virtual void Bind() override;

		private:
			void CreateDescriptorSets();

		private:
			Ref<VKShader> m_vkShader;
			std::vector<Ref<VKTexture2D>> m_textures;
			std::vector<Ref<VKUniformBuffer>> m_uniformBuffers;

			std::vector<VkDescriptorSet> m_descriptorSets;
		};
	}
}