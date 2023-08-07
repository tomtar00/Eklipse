#include "precompiled.h"
#include "VkDescriptor.h"

#include "_globals.h"
#include "VkUtils.h"
#include "VkBuffers.h"

#include <Eklipse/Renderer/Vertex.h>


namespace Eklipse
{
	namespace Vulkan
	{
		VkDescriptorSetLayout			g_graphicsDescriptorSetLayout	= VK_NULL_HANDLE;
		VkDescriptorSetLayout			g_computeDescriptorSetLayout	= VK_NULL_HANDLE;
		VkDescriptorPool				g_descriptorPool				= VK_NULL_HANDLE;
		std::vector<VkDescriptorSet>	g_graphicsDescriptorSets{};
		std::vector<VkDescriptorSet>	g_computeDescriptorSets{};

		std::vector<VkVertexInputBindingDescription> GetVertexBindingDescription()
		{
			std::vector<VkVertexInputBindingDescription> bindingDescription{1};

			bindingDescription[0].binding = 0;
			bindingDescription[0].stride = sizeof(Vertex);
			bindingDescription[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDescription;
		}
		std::vector<VkVertexInputAttributeDescription> GetVertexAttributeDescriptions()
		{
			std::vector<VkVertexInputAttributeDescription> attributeDescriptions{3};

			// position attributes
			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(Vertex, pos);

			// color attributes
			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Vertex, color);

			// texture coordinates attributes
			attributeDescriptions[2].binding = 0;
			attributeDescriptions[2].location = 2;
			attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

			return attributeDescriptions;
		}

		std::vector<VkVertexInputBindingDescription> GetParticleBindingDescription()
		{
			std::vector<VkVertexInputBindingDescription> bindingDescription{1};

			bindingDescription[0].binding = 0;
			bindingDescription[0].stride = sizeof(Particle);
			bindingDescription[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDescription;
		}
		std::vector<VkVertexInputAttributeDescription> GetParticleAttributeDescriptions()
		{
			std::vector<VkVertexInputAttributeDescription> attributeDescriptions{2};

			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(Particle, pos);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Particle, color);

			return attributeDescriptions;
		}

		void SetupDescriptorSetLayouts()
		{
			VkResult res;

			// Graphics
			{
				VkDescriptorSetLayoutBinding bindings[2];

				// Uniform buffers binding
				bindings[0].binding = 0;
				bindings[0].descriptorCount = 1;
				bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
				bindings[0].pImmutableSamplers = nullptr;

				// Image sampler binding
				bindings[1].binding = 1;
				bindings[1].descriptorCount = 1;
				bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
				bindings[1].pImmutableSamplers = nullptr;

				VkDescriptorSetLayoutCreateInfo layoutInfo{};
				layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
				layoutInfo.bindingCount = 2;
				layoutInfo.pBindings = bindings;

				res = vkCreateDescriptorSetLayout(g_logicalDevice, &layoutInfo, nullptr, &g_graphicsDescriptorSetLayout);
				HANDLE_VK_RESULT(res, "CREATE GRAPHICS DESCRIPTOR SET LAYOUT");
			}

			// Compute
			{
				VkDescriptorSetLayoutBinding layoutBindings[3];

				layoutBindings[0].binding = 0;
				layoutBindings[0].descriptorCount = 1;
				layoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				layoutBindings[0].pImmutableSamplers = nullptr;
				layoutBindings[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

				layoutBindings[1].binding = 1;
				layoutBindings[1].descriptorCount = 1;
				layoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
				layoutBindings[1].pImmutableSamplers = nullptr;
				layoutBindings[1].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

				layoutBindings[2].binding = 2;
				layoutBindings[2].descriptorCount = 1;
				layoutBindings[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
				layoutBindings[2].pImmutableSamplers = nullptr;
				layoutBindings[2].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

				VkDescriptorSetLayoutCreateInfo layoutInfo{};
				layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
				layoutInfo.bindingCount = 3;
				layoutInfo.pBindings = layoutBindings;

				res = vkCreateDescriptorSetLayout(g_logicalDevice, &layoutInfo, nullptr, &g_computeDescriptorSetLayout);
				HANDLE_VK_RESULT(res, "CREATE COMPUTE DESCRIPTOR SET LAYOUT");
			}
		}
		void DisposeDescriptorSetLayouts()
		{
			vkDestroyDescriptorSetLayout(g_logicalDevice, g_graphicsDescriptorSetLayout, nullptr);
			vkDestroyDescriptorSetLayout(g_logicalDevice, g_computeDescriptorSetLayout, nullptr);
		}
		
		void SetupDescriptorPool()
		{
			VkDescriptorPoolSize poolSizes[3] =
			{
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,			100	},
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,	100	},
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,			100	}
			};

			VkDescriptorPoolCreateInfo poolInfo{};
			poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			poolInfo.maxSets = 100;
			poolInfo.poolSizeCount = 3;
			poolInfo.pPoolSizes = poolSizes;

			VkResult res = vkCreateDescriptorPool(g_logicalDevice, &poolInfo, nullptr, &g_descriptorPool);
			HANDLE_VK_RESULT(res, "CREATE DESCRIPTOR POOL");
		}
		void DisposeDescriptorPool()
		{
			vkDestroyDescriptorPool(g_logicalDevice, g_descriptorPool, nullptr);
		}
	}
}

