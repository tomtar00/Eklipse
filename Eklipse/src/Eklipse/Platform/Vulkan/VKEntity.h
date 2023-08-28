#pragma once
#include "VKBuffers.h"
#include "VKImage.h"

#include <vulkan/vulkan.h>
#include <Eklipse/Scene/Scene.h>

namespace Eklipse
{
	namespace Vulkan
	{
		class VkEntityWrapper
		{
		public:
			VkEntityWrapper(Entity& entity);
			void Dispose();

			void Bind(VkCommandBuffer commandBuffer);
			void Draw(VkCommandBuffer commandBuffer);

			Ref<VertexBuffer> m_vertexBuffer;
			Ref<IndexBuffer> m_indexBuffer;
			Ref<UniformBuffer> m_uniformBuffer;
			Vulkan::Texture m_texture;

		private:
			void AllocateDescriptorSet();

		private:
			Entity& m_entity;
			VkDescriptorSet m_descriptorSet;
		};

		class VkEntityManager
		{
		public:
			void Setup(Scene* scene);
			void Dispose();

			std::vector<VkEntityWrapper> m_wrappers;
		};
	}
}