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
			VkPipelineCache pipelineCache;
			VkRenderPass renderPass;

			VkPrimitiveTopology topology;
			VkPolygonMode mode;

			bool depthTest;
		};
		VkPipeline CreateGraphicsPipeline(const GraphicsPipelineCreateInfo& createInfo);

		struct ComputePipelineCreateInfo
		{
			VkPipelineShaderStageCreateInfo shaderStage;
            VkPipelineLayout pipelineLayout;
			VkPipelineCache pipelineCache;
		};
		VkPipeline CreateComputePipeline(const ComputePipelineCreateInfo& createInfo);

		class EK_API VKPipeline : public Pipeline
		{
		public:
			VKPipeline(const Pipeline::Config& config);

			static void DisposeCache();

			virtual void Build() override;
			virtual void Bind() override;
			virtual void Dispose() override;

		private:
			static VkPipelineCache s_pipelineCache;
			VkPipeline m_pipeline = VK_NULL_HANDLE;
			VkPipelineBindPoint m_bindPoint;
		};
	}
}