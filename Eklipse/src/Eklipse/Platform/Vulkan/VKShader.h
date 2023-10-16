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
			virtual ~VKShader() = default;

			virtual void Bind() const override;
			virtual void Unbind() const override;
			virtual void Dispose() const override;

			VkDescriptorSetLayout GetDescriptorSetLayout() const { return m_descriptorSetLayout; }
			VkPipelineLayout GetPipelineLayout() const { return m_pipelineLayout; }

		protected:
			virtual const std::string GetCacheDirectoryPath() override { return "Assets/Cache/Shader/Vulkan"; }

		private:
			VkPipeline m_pipeline;
			VkPipelineLayout m_pipelineLayout;
			VkDescriptorSetLayout m_descriptorSetLayout;
		};
	}
}