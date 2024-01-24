#pragma once
#include <Eklipse/Renderer/Shader.h>
#include <vulkan/vulkan.h>

namespace Eklipse
{
	namespace Vulkan
	{
		VkShaderStageFlagBits VKShaderStageFromInternalStage(const ShaderStage stage);
		VkFormat VertexInputSizeToVKFormat(const size_t size);

		class VKShader : public Shader
		{
		public:
			VKShader(const Path& filePath);

			VkDescriptorSetLayout GetDescriptorSetLayout() const;
			VkPipelineLayout GetPipelineLayout() const;

			virtual void Bind() const override;
			virtual void Unbind() const override;
			virtual void Dispose() const override;

		protected:
			virtual const std::string GetCacheDirectoryPath() override;
			virtual bool Compile(bool forceCompile = false) override;

		private:
			VkPipeline m_pipeline;
			VkPipelineLayout m_pipelineLayout;
			VkDescriptorSetLayout m_descriptorSetLayout;
		};
	}
}