#pragma once
#include <vulkan/vulkan.h>
#include <Eklipse/Renderer/Pipeline.h>

namespace Eklipse
{
	namespace Vulkan
	{
		VkPipelineLayout CreatePipelineLayout(Vec<VkDescriptorSetLayout> descSetLayouts, Vec<VkPushConstantRange> pushConstantRanges);

		struct GraphicsPipelineCreateInfo
		{
			Vec<VkPipelineShaderStageCreateInfo> shaderStages;
			Vec<VkVertexInputBindingDescription> bindingDesc;
			Vec<VkVertexInputAttributeDescription> attribteDesc;

			VkPipelineLayout pipelineLayout;
			VkRenderPass renderPass;

			VkPrimitiveTopology topology;
			VkPolygonMode mode;
		};
		VkPipeline CreateGraphicsPipeline(const GraphicsPipelineCreateInfo& createInfo);

		struct ComputePipelineCreateInfo
		{
			
		};
		VkPipeline CreateComputePipeline(const ComputePipelineCreateInfo& createInfo);


		class EK_API VKPipeline : public Pipeline
		{
		public:
			VKPipeline(const Pipeline::Config& config);

			virtual void Build() override;
			virtual void Bind() override;
			virtual void Dispose() override;

		private:
			VkPipeline m_pipeline = VK_NULL_HANDLE;
			VkPipelineBindPoint m_bindPoint;
		};
	}
}