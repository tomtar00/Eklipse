#pragma once
#include <Eklipse/Renderer/Shader.h>
#include <vulkan/vulkan.h>

namespace Eklipse
{
	namespace Vulkan
	{
		VkShaderStageFlagBits VKShaderStageFromInternalStage(const ShaderStage stage);
		VkFormat VertexInputSizeToVKFormat(const size_t size);

		Vec<VkVertexInputAttributeDescription> CreateVertexInputAttributeDescriptions(const ShaderReflection& vertexShaderReflection);
		Vec<VkVertexInputBindingDescription> CreateVertexInputBindingDescriptions(const ShaderReflection& vertexShaderReflection);
		Vec<VkPipelineShaderStageCreateInfo> CreateShaderStages(VkShaderModule vertexShaderModule, VkShaderModule framentShaderModule);
		VkPipelineShaderStageCreateInfo CreateShaderStage(VkShaderModule computeShaderModule);

		class VKShader : public Shader
		{
		public:
			VKShader(const String& vertexSource, const String& fragmentSource, const AssetHandle handle = AssetHandle());
			VKShader(const Path& filePath, const AssetHandle handle = AssetHandle());

			VkShaderModule GetVertexShaderModule() const;
			VkShaderModule GetFragmentShaderModule() const;
			VkShaderModule GetComputeShaderModule() const;
			VkDescriptorSetLayout GetDescriptorSetLayout() const;
			VkPipelineLayout GetPipelineLayout() const;

			virtual void Bind() const override;
			virtual void Unbind() const override;
			virtual void Dispose() override;

			virtual bool Compile(const Path& shaderPath, bool forceCompile = false) override;
			virtual bool Compile(const StageSourceMap& sourceMap, bool forceCompile = false) override;

		protected:
			virtual bool Compile(const Path& shaderPath, const StageSourceMap& sourceMap, bool forceCompile = false) override;
			virtual const String GetCacheDirectoryPath() override;

		private:
			VkShaderModule m_vertexShaderModule			= VK_NULL_HANDLE;
			VkShaderModule m_fragmentShaderModule		= VK_NULL_HANDLE;
			VkShaderModule m_computeShaderModule		= VK_NULL_HANDLE;
			VkPipelineLayout m_pipelineLayout			= VK_NULL_HANDLE;
			VkDescriptorSetLayout m_descriptorSetLayout = VK_NULL_HANDLE;
		};
	}
}