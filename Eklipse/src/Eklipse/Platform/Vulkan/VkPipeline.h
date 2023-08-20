#pragma once
#include <vulkan/vulkan.h>

namespace Eklipse
{
	namespace Vulkan
	{
		VkPipeline CreateGraphicsPipeline(const char* vertShaderRelPath, const char* fragShaderRelPath,
			VkPipelineLayout& pipelineLayout, VkRenderPass renderPass,
			std::vector<VkVertexInputBindingDescription> vertBindingDesc,
			std::vector<VkVertexInputAttributeDescription> vertAttribteDesc,
			VkDescriptorSetLayout* descSetLayouts);

		VkPipeline CreateComputePipeline(const char* shaderRelPath, VkPipelineLayout pipelineLayout, 
			VkDescriptorSetLayout* descSetLayout);

		VkRenderPass CreateRenderPass();
		VkRenderPass CreateViewportRenderPass();
		VkRenderPass CreateImGuiRenderPass();
	}
}