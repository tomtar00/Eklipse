#include "precompiled.h"

#include "VK.h"
#include "VKShader.h"
#include "VKUtils.h"

#include <Eklipse/Utils/File.h>
#include "VKDescriptor.h"
#include "VKPipeline.h"

namespace Eklipse
{
	namespace Vulkan
	{
		VKShader::VKShader(const std::string& name, const std::string& vertPath, const std::string& fragPath)
		{
			auto vertShaderCode = Eklipse::ReadFileFromPath(vertPath);
			VkShaderModule vertShaderModule = CreateShaderModule(vertShaderCode);
			VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
			vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
			vertShaderStageInfo.module = vertShaderModule;
			vertShaderStageInfo.pName = "main";

			auto fragShaderCode = Eklipse::ReadFileFromPath(fragPath);
			VkShaderModule fragShaderModule = CreateShaderModule(fragShaderCode);
			VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
			fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			fragShaderStageInfo.module = fragShaderModule;
			fragShaderStageInfo.pName = "main";

			std::vector<VkPipelineShaderStageCreateInfo> shaderStages = { vertShaderStageInfo, fragShaderStageInfo };

			m_descriptorSetLayout = CreateDescriptorSetLayout({
				{ 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, nullptr },
				{ 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr }
			});

			m_pipelineLayout = CreatePipelineLayout({ m_descriptorSetLayout });

			/*std::vector<VkVertexInputBindingDescription> bindingDescription = {
				{ 0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX }
			};
			std::vector<VkVertexInputAttributeDescription> attributeDescription = {
				{ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos) },
				{ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color) },
				{ 2, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, texCoord) }
			};
			m_pipeline = CreateGraphicsPipeline(shaderStages, m_pipelineLayout, m_renderPass, bindingDescription, attributeDescription);*/

			vkDestroyShaderModule(g_logicalDevice, fragShaderModule, nullptr);
			vkDestroyShaderModule(g_logicalDevice, vertShaderModule, nullptr);
		}

		void VKShader::Bind() const {}
		void VKShader::Unbind() const {}

		void VKShader::Dispose() const
		{
			vkDestroyPipelineLayout(g_logicalDevice, m_pipelineLayout, nullptr);
			vkDestroyDescriptorSetLayout(g_logicalDevice, m_descriptorSetLayout, nullptr);
			vkDestroyPipeline(g_logicalDevice, m_pipeline, nullptr);
		}

		void VKShader::UploadMat4(const std::string& name, const glm::mat4& matrix)
		{
			/*auto binding = GetUniformBinding(name);
			auto offset = binding.second * sizeof(glm::mat4);
			void* data;
			vkMapMemory(m_device->GetDevice(), m_uniformBuffersMemory[0], offset, sizeof(glm::mat4), 0, &data);
			memcpy(data, &matrix, sizeof(glm::mat4));
			vkUnmapMemory(m_device->GetDevice(), m_uniformBuffersMemory[0]);
			m_dirtyUniformBuffer = true;*/
		}

		void VKShader::UploadInt(const std::string& name, const int value)
		{
			/*auto binding = GetUniformBinding(name);
			auto offset = binding.second * sizeof(int);
			void* data;
			vkMapMemory(m_device->GetDevice(), m_uniformBuffersMemory[1], offset, sizeof(int), 0, &data);
			memcpy(data, &value, sizeof(int));
			vkUnmapMemory(m_device->GetDevice(), m_uniformBuffersMemory[1]);
			m_dirtyUniformBuffer = true;*/
		}
	}
}
