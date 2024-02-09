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
			if (stage == ShaderStage::VERTEX)   return VK_SHADER_STAGE_VERTEX_BIT;
			if (stage == ShaderStage::FRAGMENT) return VK_SHADER_STAGE_FRAGMENT_BIT;

			EK_ASSERT(false, "Unknown shader stage! ({0})", (int)stage);
			return VK_SHADER_STAGE_ALL;
		}
		VkFormat VertexInputSizeToVKFormat(const size_t size)
		{
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

		VKShader::VKShader(const Path& filePath, const AssetHandle handle) : Shader(filePath, handle)
		{		
			m_isValid = Compile(filePath);
		}

		VkDescriptorSetLayout VKShader::GetDescriptorSetLayout() const
		{
			return m_descriptorSetLayout;
		}
		VkPipelineLayout VKShader::GetPipelineLayout() const
		{
			return m_pipelineLayout;
		}

		void VKShader::Bind() const 
		{
			EK_PROFILE();

#if EK_DEBUG
			if (m_isValid)
#endif
			vkCmdBindPipeline(g_currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
		}
		void VKShader::Unbind() const {}
		void VKShader::Dispose()
		{
			if (m_isValid)
			{
				vkDestroyPipelineLayout(g_logicalDevice, m_pipelineLayout, nullptr);
				vkDestroyDescriptorSetLayout(g_logicalDevice, m_descriptorSetLayout, nullptr);
				vkDestroyPipeline(g_logicalDevice, m_pipeline, nullptr);
			}
		}

		const String VKShader::GetCacheDirectoryPath()
		{
			return "Assets/Cache/Shader/Vulkan";
		}
		bool VKShader::Compile(const Path& shaderPath, bool forceCompile)
		{
			EK_CORE_TRACE("Compiling Vulkan shader '{0}'", Name);

			auto shaderSources = Setup(shaderPath);
			bool success = true;

			Timer timer;
			success = success && CompileOrGetVulkanBinaries(shaderPath, shaderSources, forceCompile);
			if (success)
			{
				// Create modules
				auto& vertShaderCode = m_vulkanSPIRV[ShaderStage::VERTEX];
				VkShaderModule vertShaderModule = CreateShaderModule(vertShaderCode);
				VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
				vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
				vertShaderStageInfo.module = vertShaderModule;
				vertShaderStageInfo.pName = "main";

				auto& fragShaderCode = m_vulkanSPIRV[ShaderStage::FRAGMENT];
				VkShaderModule fragShaderModule = CreateShaderModule(fragShaderCode);
				VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
				fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
				fragShaderStageInfo.module = fragShaderModule;
				fragShaderStageInfo.pName = "main";

				Vec<VkPipelineShaderStageCreateInfo> shaderStages = { vertShaderStageInfo, fragShaderStageInfo };

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

				// Create pipeline
				size_t inputSize = 0;
				for (auto& vertexReflection : m_reflections[ShaderStage::VERTEX].inputs)
					inputSize += vertexReflection.size;

				Vec<VkVertexInputBindingDescription> bindingDescription;
				VkVertexInputBindingDescription vertexInputDescription{};
				vertexInputDescription.binding = 0;
				vertexInputDescription.stride = inputSize;
				vertexInputDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
				bindingDescription.push_back(vertexInputDescription);

				Vec<VkVertexInputAttributeDescription> attributeDescription;
				for (auto& vertexReflection : m_reflections[ShaderStage::VERTEX].inputs)
				{
					VkVertexInputAttributeDescription attribute{};
					attribute.binding = 0;
					attribute.location = vertexReflection.location;
					attribute.format = VertexInputSizeToVKFormat(vertexReflection.size);
					attribute.offset = vertexReflection.offset;
					attributeDescription.push_back(attribute);
				}

				if (g_VKOffScreenFramebuffers.size() <= 0)
				{
					m_pipeline = CreateGraphicsPipeline(shaderStages, m_pipelineLayout, g_VKDefaultFramebuffer->GetRenderPass(), bindingDescription, attributeDescription);
				}
				else
					m_pipeline = CreateGraphicsPipeline(shaderStages, m_pipelineLayout, g_VKOffScreenFramebuffers[0]->GetRenderPass(), bindingDescription, attributeDescription);

				vkDestroyShaderModule(g_logicalDevice, fragShaderModule, nullptr);
				vkDestroyShaderModule(g_logicalDevice, vertShaderModule, nullptr);

				EK_CORE_DBG("Creation of shader '{0}' took {1} ms", Name, timer.ElapsedTimeMs());
			}
			else EK_CORE_ERROR("Shader {0} compilation failed", Handle);
			
			return success;
		}
	}
}