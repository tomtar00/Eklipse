#pragma once
#include <vulkan/vulkan.h>

namespace Eklipse
{
	namespace Vulkan
	{
		VkPipeline CreateGraphicsPipeline(Vec<VkPipelineShaderStageCreateInfo> shaderStages,
			VkPipelineLayout pipelineLayout, VkRenderPass renderPass,
			Vec<VkVertexInputBindingDescription> bindingDesc,
			Vec<VkVertexInputAttributeDescription> attribteDesc);

		VkPipelineLayout CreatePipelineLayout(Vec<VkDescriptorSetLayout> descSetLayouts, Vec<VkPushConstantRange> pushConstantRanges);

		VkPipeline CreateComputePipeline(const char* shaderRelPath, VkPipelineLayout pipelineLayout, 
			VkDescriptorSetLayout* descSetLayout);

		VkRenderPass CreateRenderPass();
		VkRenderPass CreateViewportRenderPass();
		VkRenderPass CreateImGuiRenderPass();
	}
}