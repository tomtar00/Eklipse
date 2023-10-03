#include "precompiled.h"
#include "VKShader.h"
#include "VKUtils.h"
#include <Eklipse/Utils/File.h>

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
		}

		void VKShader::Bind() const {}
		void VKShader::Unbind() const {}

		void VKShader::Dispose() const
		{
			/*if (m_pipelineLayout) vkDestroyPipelineLayout(m_device->GetDevice(), m_pipelineLayout, nullptr);
			if (m_pipeline) vkDestroyPipeline(m_device->GetDevice(), m_pipeline, nullptr);
			if (m_valid) {
				for (auto module : m_shaderModules) {
					vkDestroyShaderModule(m_device->GetDevice(), module, nullptr);
				}
				m_shaderModules.clear();
			}*/
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
