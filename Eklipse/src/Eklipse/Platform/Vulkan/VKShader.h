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
			VKShader(const std::string& name, const std::string& vertPath, const std::string& fragPath);

			virtual void Bind() const override;
			virtual void Unbind() const override;
			virtual void Dispose() const override;

			virtual void UploadMat4(const std::string& name, const glm::mat4& matrix) override;
			virtual void UploadInt(const std::string& name, const int value) override;

		private:
			VkPipeline m_pipeline;
			VkPipelineLayout m_pipelineLayout;
			VkDescriptorSetLayout m_descriptorSetLayout;
			VkRenderPass m_renderPass;
		};
	}
}