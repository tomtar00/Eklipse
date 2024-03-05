#include "precompiled.h"

#include "VK.h"
#include "VKShader.h"
#include "VKUtils.h"
#include "VKDescriptor.h"
#include "VKPipeline.h"

#include <filesystem>
#include <fstream>
#include <shaderc/shaderc.hpp>
#include <spirv_cross.hpp>
#include <spirv_glsl.hpp>
#include <Eklipse/Core/Timer.h>

namespace Eklipse
{
	namespace Vulkan
	{
		VkShaderStageFlagBits VKShaderStageFromInternalStage(const ShaderStage stage)
		{
			EK_CORE_PROFILE();
			if (stage == ShaderStage::VERTEX)   return VK_SHADER_STAGE_VERTEX_BIT;
			if (stage == ShaderStage::FRAGMENT) return VK_SHADER_STAGE_FRAGMENT_BIT;
			if (stage == ShaderStage::COMPUTE) return VK_SHADER_STAGE_COMPUTE_BIT;

			EK_ASSERT(false, "Unknown shader stage! ({0})", (int)stage);
			return VK_SHADER_STAGE_ALL;
		}
		VkFormat VertexInputSizeToVKFormat(const size_t size)
		{
			EK_CORE_PROFILE();
			switch (size)
			{
				case sizeof(float):		return VK_FORMAT_R32_SFLOAT;
				case sizeof(glm::vec2): return VK_FORMAT_R32G32_SFLOAT;
				case sizeof(glm::vec3): return VK_FORMAT_R32G32B32_SFLOAT;
				case sizeof(glm::vec4): return VK_FORMAT_R32G32B32A32_SFLOAT;
			}
			EK_ASSERT(false, "Unknown vertex input size! ({0})", size);
			return VK_FORMAT_UNDEFINED;
		}

		Vec<VkVertexInputAttributeDescription> CreateVertexInputAttributeDescriptions(const ShaderReflection& vertexShaderReflection)
		{
			Vec<VkVertexInputAttributeDescription> attributeDescription;
			for (auto& vertexReflection : vertexShaderReflection.inputs)
			{
				VkVertexInputAttributeDescription attribute{};
				attribute.binding = 0;
				attribute.location = vertexReflection.location;
				attribute.format = VertexInputSizeToVKFormat(vertexReflection.size);
				attribute.offset = vertexReflection.offset;
				attributeDescription.push_back(attribute);
			}
			return attributeDescription;
		}
		Vec<VkVertexInputBindingDescription> CreateVertexInputBindingDescriptions(const ShaderReflection& vertexShaderReflection)
		{
			size_t inputSize = 0;
			for (auto& vertexReflection : vertexShaderReflection.inputs)
				inputSize += vertexReflection.size;

			Vec<VkVertexInputBindingDescription> bindingDescription;
			VkVertexInputBindingDescription vertexInputDescription{};
			vertexInputDescription.binding = 0;
			vertexInputDescription.stride = inputSize;
			vertexInputDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			bindingDescription.push_back(vertexInputDescription);

			return bindingDescription;
		}

		Vec<VkPipelineShaderStageCreateInfo> CreateShaderStages(VkShaderModule vertexShaderModule, VkShaderModule framentShaderModule)
		{
			VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
			vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
			vertShaderStageInfo.module = vertexShaderModule;
			vertShaderStageInfo.pName = "main";

			VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
			fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			fragShaderStageInfo.module = framentShaderModule;
			fragShaderStageInfo.pName = "main";

			return Vec<VkPipelineShaderStageCreateInfo>{ vertShaderStageInfo, fragShaderStageInfo };
		}
		VkPipelineShaderStageCreateInfo CreateShaderStage(VkShaderModule computeShaderModule)
		{
			VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
			vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			vertShaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
			vertShaderStageInfo.module = computeShaderModule;
			vertShaderStageInfo.pName = "main";

			return vertShaderStageInfo;
		}

		VKShader::VKShader(const String& vertexSource, const String& fragmentSource, const AssetHandle handle) 
			: Shader(vertexSource, fragmentSource, handle)
		{
			EK_CORE_PROFILE();
			Handle = handle;
			Name = "Custom";
			StageSourceMap shaderSources;
			shaderSources[ShaderStage::VERTEX] = vertexSource;
			shaderSources[ShaderStage::FRAGMENT] = fragmentSource;
			Compile(shaderSources);
		}
		VKShader::VKShader(const Path& filePath, const AssetHandle handle) 
			: Shader(filePath, handle)
		{		
			EK_CORE_PROFILE();
			Handle = handle;
			Name = filePath.stem().string();
			Compile(filePath);
		}

		VkShaderModule VKShader::GetVertexShaderModule() const
		{
			return m_vertexShaderModule;
		}
		VkShaderModule VKShader::GetFragmentShaderModule() const
		{
			return m_fragmentShaderModule;
		}
		VkShaderModule VKShader::GetComputeShaderModule() const
		{
			return m_computeShaderModule;
		}
		VkDescriptorSetLayout VKShader::GetDescriptorSetLayout() const
		{
			return m_descriptorSetLayout;
		}
		VkPipelineLayout VKShader::GetPipelineLayout() const
		{
			return m_pipelineLayout;
		}
		VkPipelineBindPoint VKShader::GetPipelineBindPoint() const
		{
			return m_pipelineBindPoint;
		}

		void VKShader::Bind() const 
		{
			EK_CORE_WARN("VKShader::Bind() not implemented. Call Bind() on Pipeline object instead");
		}
		void VKShader::Unbind() const {}
		void VKShader::Dispose()
		{
			EK_CORE_PROFILE();
			EK_CORE_TRACE("Disposing Vulkan shader '{0}'", Name);
			if (m_vertexShaderModule != VK_NULL_HANDLE)
                vkDestroyShaderModule(g_logicalDevice, m_vertexShaderModule, nullptr);
			if (m_fragmentShaderModule != VK_NULL_HANDLE)
				vkDestroyShaderModule(g_logicalDevice, m_fragmentShaderModule, nullptr);
			if (m_computeShaderModule != VK_NULL_HANDLE)
                vkDestroyShaderModule(g_logicalDevice, m_computeShaderModule, nullptr);
			if (m_pipelineLayout != VK_NULL_HANDLE)
				vkDestroyPipelineLayout(g_logicalDevice, m_pipelineLayout, nullptr);
			if (m_descriptorSetLayout != VK_NULL_HANDLE)
				vkDestroyDescriptorSetLayout(g_logicalDevice, m_descriptorSetLayout, nullptr);
			EK_CORE_DBG("Disposed Vulkan shader '{0}'", Name);
		}

		bool VKShader::Compile(const Path& shaderPath, bool forceCompile)
		{
			EK_CORE_PROFILE();
			auto shaderSources = Setup(shaderPath);
			return Compile(shaderPath, shaderSources, forceCompile);
		}
		bool VKShader::Compile(const StageSourceMap& sourceMap, bool forceCompile)
		{
			EK_CORE_PROFILE();
			return Compile("", sourceMap, forceCompile);
		}
		bool VKShader::Compile(const Path& shaderPath, const StageSourceMap& sourceMap, bool forceCompile)
		{
			EK_CORE_TRACE("Compiling Vulkan shader '{0}'", Name);

			Timer _timer;
			bool success = CompileOrGetVulkanBinaries(shaderPath, sourceMap, forceCompile);
			if (success)
			{
				Dispose();

				// Create modules
				if (m_vulkanSPIRV.find(ShaderStage::VERTEX) != m_vulkanSPIRV.end())
                    m_vertexShaderModule = CreateShaderModule(m_vulkanSPIRV[ShaderStage::VERTEX]);

				if (m_vulkanSPIRV.find(ShaderStage::FRAGMENT) != m_vulkanSPIRV.end())
					m_fragmentShaderModule = CreateShaderModule(m_vulkanSPIRV[ShaderStage::FRAGMENT]);

				if (m_vulkanSPIRV.find(ShaderStage::COMPUTE) != m_vulkanSPIRV.end())
				{
					m_pipelineBindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;
                    m_computeShaderModule = CreateShaderModule(m_vulkanSPIRV[ShaderStage::COMPUTE]);
				}

				// Create descriptor set layout
				Vec<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;
				for (auto&& [stage, reflection] : m_reflections)
				{
					for (auto& uniformBuffer : reflection.uniformBuffers)
					{
						VkDescriptorSetLayoutBinding uboLayoutBinding{};
						uboLayoutBinding.binding = uniformBuffer.binding;
						uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
						uboLayoutBinding.descriptorCount = 1;
						uboLayoutBinding.stageFlags = VKShaderStageFromInternalStage(stage);
						uboLayoutBinding.pImmutableSamplers = nullptr;
						descriptorSetLayoutBindings.push_back(uboLayoutBinding);
					}
					for (auto& storageBuffer : reflection.storageBuffers)
					{
						VkDescriptorSetLayoutBinding storageLayoutBinding{};
						storageLayoutBinding.binding = storageBuffer.binding;
						storageLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
						storageLayoutBinding.descriptorCount = 1;
						storageLayoutBinding.stageFlags = VKShaderStageFromInternalStage(stage);
						storageLayoutBinding.pImmutableSamplers = nullptr;
						descriptorSetLayoutBindings.push_back(storageLayoutBinding);
					}
					for (auto& sampler : reflection.samplers)
					{
						VkDescriptorSetLayoutBinding samplerLayoutBinding{};
						samplerLayoutBinding.binding = sampler.binding;
						samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
						samplerLayoutBinding.descriptorCount = 1;
						samplerLayoutBinding.stageFlags = VKShaderStageFromInternalStage(stage);
						samplerLayoutBinding.pImmutableSamplers = nullptr;
						descriptorSetLayoutBindings.push_back(samplerLayoutBinding);
					}
				}
				m_descriptorSetLayout = CreateDescriptorSetLayout(descriptorSetLayoutBindings);

				// Create push constant ranges
				Vec<VkPushConstantRange> pushConstantRanges;
				for (auto&& [stage, reflection] : m_reflections)
				{
					for (auto& pushConstant : reflection.pushConstants)
					{
						VkPushConstantRange range{};
						range.offset = pushConstant.offset;
						range.size = pushConstant.size;
						range.stageFlags = VKShaderStageFromInternalStage(stage);

						pushConstantRanges.push_back(range);
					}
				}

				// Create pipeline layout
				m_pipelineLayout = CreatePipelineLayout({ m_descriptorSetLayout }, pushConstantRanges);

				// Rebuild pipelines
				Vec<Ref<Pipeline>> pipelines = Pipeline::GetPipelinesByShader(Handle);
				for (auto& pipeline : pipelines)
                {
                    pipeline->Dispose();
                    pipeline->Build();
                }

				EK_CORE_DBG("Creation of shader '{0}' took {1} ms", Name, _timer.ElapsedTimeMs());
			}
			else EK_CORE_ERROR("Shader {0} compilation failed", Handle);
			return success;
		}
		const String VKShader::GetCacheDirectoryPath()
		{
			return "Assets/Cache/Shader/Vulkan";
		}
	}
}