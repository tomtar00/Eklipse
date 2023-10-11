#pragma once
#include <Eklipse/Renderer/Shader.h>
#include <vulkan/vulkan.h>

namespace Eklipse
{
	namespace Vulkan
	{
		class VKShader : public Eklipse::Shader
		{
		public:
			VKShader(const std::string& filePath);

			virtual void Bind() const override;
			virtual void Unbind() const override;
			virtual void Dispose() const override;

			VkDescriptorSetLayout GetDescriptorSetLayout() const { return m_descriptorSetLayout; }
			VkPipelineLayout GetPipelineLayout() const { return m_pipelineLayout; }

		private:
			void CompileOrGetVulkanBinaries(const std::unordered_map<ShaderStage, std::string>& shaderSources);

		private:
			VkPipeline m_pipeline;
			VkPipelineLayout m_pipelineLayout;
			VkDescriptorSetLayout m_descriptorSetLayout;

			std::string m_filePath;
			std::unordered_map<ShaderStage, std::vector<uint32_t>> m_vulkanSPIRV;
		};
	}
}