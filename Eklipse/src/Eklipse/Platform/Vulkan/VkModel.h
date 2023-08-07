#pragma once
#include "VkBuffers.h"
#include "VkImage.h"

#include <vulkan/vulkan.h>
#include <Eklipse/Scene/Model.h>
#include <Eklipse/Scene/Scene.h>

namespace Eklipse
{
	namespace Vulkan
	{
		class ModelAdapter
		{
		public:
			ModelAdapter(Model& model);
			void Dispose();

			void Bind(VkCommandBuffer commandBuffer);
			void Draw(VkCommandBuffer commandBuffer);

			Vulkan::UniformBuffer m_uniformBuffer;
			Vulkan::VertexBuffer m_vertexBuffer;
			Vulkan::IndexBuffer m_indexBuffer;
			Vulkan::Texture m_texture;

		private:
			void AllocateDescriptorSet();

		private:
			Model& m_model;
			VkDescriptorSet m_descriptorSet;
		};

		class ModelManager
		{
		public:
			void Setup(Scene* scene);
			void Dispose();

			std::vector<ModelAdapter> m_models;
		};
	}
}