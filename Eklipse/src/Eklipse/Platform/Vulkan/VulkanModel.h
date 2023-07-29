#pragma once
#include "VulkanBuffers.h"
#include "VulkanTexture.h"

#include <vulkan/vulkan.h>

namespace Eklipse
{
	class VulkanModel
	{
	public:
		void Load(char* modelPath, char* texturePath);
		void Shutdown();

		VulkanVertexBuffer& VertexBuffer();
		VulkanIndexBuffer& IndexBuffer();
		VulkanTexture& Texture();

	private:
		VulkanVertexBuffer m_vertexBuffer;
		VulkanIndexBuffer m_indexBuffer;
		VulkanTexture m_texture;
	};
}