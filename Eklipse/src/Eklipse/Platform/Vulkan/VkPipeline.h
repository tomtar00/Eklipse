#pragma once
#include <vulkan/vulkan.h>

namespace Eklipse
{
	namespace Vulkan
	{
		void CreateGraphicsPipeline(const char* vertShaderRelPath, const char* fragShaderRelPath,
			VkPipelineLayout& pipelineLayout, VkPipeline& pipeline, VkRenderPass& renderPass,
			std::vector<VkVertexInputBindingDescription> vertBindingDesc,
			std::vector<VkVertexInputAttributeDescription> vertAttribteDesc,
			VkDescriptorSetLayout* descSetLayouts);
		void CreateComputePipeline(const char* shaderRelPath, VkPipelineLayout& pipelineLayout,
			VkPipeline& pipeline, VkDescriptorSetLayout* descSetLayout);
		void CreateRenderPass(VkRenderPass& renderPass);

		void SetupPipelines();
		void DisposePipelines();
	}
}