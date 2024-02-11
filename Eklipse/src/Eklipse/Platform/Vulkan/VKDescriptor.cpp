#include "precompiled.h"
#include "VKDescriptor.h"
		   
#include "VK.h"
#include "VKUtils.h"
#include "VKBuffers.h"

#include <Eklipse/Renderer/Vertex.h>

namespace Eklipse
{
	namespace Vulkan
	{
		//VkDescriptorSetLayout			g_graphicsDescriptorSetLayout	= VK_NULL_HANDLE;
		//VkDescriptorSetLayout			g_computeDescriptorSetLayout	= VK_NULL_HANDLE;
		VkDescriptorPool				g_descriptorPool				= VK_NULL_HANDLE;
		//Vec<VkDescriptorSet>	g_graphicsDescriptorSets{};
		//Vec<VkDescriptorSet>	g_computeDescriptorSets{};

		//Vec<VkVertexInputBindingDescription> GetVertexBindingDescription()
		//{
		//	Vec<VkVertexInputBindingDescription> bindingDescription{1};

		//	bindingDescription[0].binding = 0;
		//	bindingDescription[0].stride = sizeof(Vertex);
		//	bindingDescription[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		//	return bindingDescription;
		//}
		//Vec<VkVertexInputAttributeDescription> GetVertexAttributeDescriptions()
		//{
		//	Vec<VkVertexInputAttributeDescription> attributeDescriptions{3};

		//	// position attributes
		//	attributeDescriptions[0].binding = 0;
		//	attributeDescriptions[0].location = 0;
		//	attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		//	attributeDescriptions[0].offset = offsetof(Vertex, pos);

		//	// color attributes
		//	attributeDescriptions[1].binding = 0;
		//	attributeDescriptions[1].location = 1;
		//	attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		//	attributeDescriptions[1].offset = offsetof(Vertex, color);

		//	// texture coordinates attributes
		//	attributeDescriptions[2].binding = 0;
		//	attributeDescriptions[2].location = 2;
		//	attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		//	attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

		//	return attributeDescriptions;
		//}

		//Vec<VkVertexInputBindingDescription> GetParticleBindingDescription()
		//{
		//	Vec<VkVertexInputBindingDescription> bindingDescription{1};

		//	bindingDescription[0].binding = 0;
		//	bindingDescription[0].stride = sizeof(Particle);
		//	bindingDescription[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		//	return bindingDescription;
		//}
		//Vec<VkVertexInputAttributeDescription> GetParticleAttributeDescriptions()
		//{
		//	Vec<VkVertexInputAttributeDescription> attributeDescriptions{2};

		//	attributeDescriptions[0].binding = 0;
		//	attributeDescriptions[0].location = 0;
		//	attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		//	attributeDescriptions[0].offset = offsetof(Particle, pos);

		//	attributeDescriptions[1].binding = 0;
		//	attributeDescriptions[1].location = 1;
		//	attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		//	attributeDescriptions[1].offset = offsetof(Particle, color);

		//	return attributeDescriptions;
		//}

		VkDescriptorSetLayout CreateDescriptorSetLayout(Vec<VkDescriptorSetLayoutBinding> bindings)
		{
			EK_CORE_PROFILE();
			VkDescriptorSetLayoutCreateInfo layoutInfo{};
			layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
			layoutInfo.pBindings = bindings.data();

			VkDescriptorSetLayout layout;
			VkResult res = vkCreateDescriptorSetLayout(g_logicalDevice, &layoutInfo, nullptr, &layout);
			HANDLE_VK_RESULT(res, "CREATE DESCRIPTOR SET LAYOUT");

			return layout;
		}
		VkDescriptorPool CreateDescriptorPool(Vec<VkDescriptorPoolSize> poolSizes, int maxSets, VkDescriptorPoolCreateFlags flags)
		{
			EK_CORE_PROFILE();
			VkDescriptorPoolCreateInfo poolInfo{};
			poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			poolInfo.flags = flags;
			poolInfo.maxSets = maxSets;
			poolInfo.poolSizeCount = poolSizes.size();
			poolInfo.pPoolSizes = poolSizes.data();

			VkDescriptorPool pool;
			VkResult res = vkCreateDescriptorPool(g_logicalDevice, &poolInfo, nullptr, &pool);
			HANDLE_VK_RESULT(res, "CREATE DESCRIPTOR POOL");

			return pool;
		}
	}
}

