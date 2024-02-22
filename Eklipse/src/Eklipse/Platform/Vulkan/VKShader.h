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
			VKShader(const String& vertexSource, const String& fragmentSource, const AssetHandle handle = AssetHandle());
			VKShader(const Path& filePath, const AssetHandle handle = AssetHandle());

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
			VkPipeline m_pipeline = VK_NULL_HANDLE;
			VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
			VkDescriptorSetLayout m_descriptorSetLayout = VK_NULL_HANDLE;
		};
	}
}