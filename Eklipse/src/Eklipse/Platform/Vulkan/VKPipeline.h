#pragma once
#include <vulkan/vulkan.h>

namespace Eklipse
{
	namespace Vulkan
	{
		VkPipeline CreateGraphicsPipeline(std::vector<VkPipelineShaderStageCreateInfo> shaderStages,
			VkPipelineLayout pipelineLayout, VkRenderPass renderPass,
			std::vector<VkVertexInputBindingDescription> bindingDesc,
			std::vector<VkVertexInputAttributeDescription> attribteDesc);

		VkPipelineLayout CreatePipelineLayout(std::vector<VkDescriptorSetLayout> descSetLayouts);

		VkPipeline CreateComputePipeline(const char* shaderRelPath, VkPipelineLayout pipelineLayout, 
			VkDescriptorSetLayout* descSetLayout);

		VkRenderPass CreateRenderPass();
		VkRenderPass CreateViewportRenderPass();
		VkRenderPass CreateImGuiRenderPass();
	}
}