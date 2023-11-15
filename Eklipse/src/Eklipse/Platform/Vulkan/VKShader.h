#pragma once
#include <Eklipse/Renderer/Shader.h>
#include <vulkan/vulkan.h>

namespace Eklipse
{
	namespace Vulkan
	{
		extern VkShaderStageFlagBits VKShaderStageFromInternalStage(const ShaderStage stage);
		extern VkFormat VertexInputSizeToVKFormat(const size_t size);

		class VKShader : public Eklipse::Shader
		{
		public:
			VKShader(const Path& filePath);
			virtual ~VKShader() = default;

			virtual void Bind() const override;
			virtual void Unbind() const override;
			virtual void Dispose() const override;

			VkDescriptorSetLayout GetDescriptorSetLayout() const { return m_descriptorSetLayout; }
			VkPipelineLayout GetPipelineLayout() const { return m_pipelineLayout; }

		protected:
			virtual const std::string GetCacheDirectoryPath() override { return "Assets/Cache/Shader/Vulkan"; }
		
		private:
			virtual bool Compile(bool forceCompile = false) override;

		private:
			VkPipeline m_pipeline;
			VkPipelineLayout m_pipelineLayout;
			VkDescriptorSetLayout m_descriptorSetLayout;
		};
	}
}