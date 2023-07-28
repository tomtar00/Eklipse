#pragma once

#include <Eklipse/Renderer/Pipeline.h>
#include <vulkan/vulkan.h>

namespace Eklipse
{
	class VulkanPipeline : public Pipeline
	{
	public:
		VulkanPipeline();
		~VulkanPipeline() override;

		void Init();
		void Shutdown();

	public:
		VkPipeline& GraphicsPipeline();
		VkPipelineLayout& Layout();
		VkRenderPass& RenderPass();

	private:
		void CreateGraphicsPipeline();
		void CreateRenderPass();

	private:
		VkRenderPass m_renderPass{};
		VkPipelineLayout m_pipelineLayout{};
		VkPipeline m_graphicsPipeline{};
	};
}