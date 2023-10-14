#include "precompiled.h"

#include "VK.h"
#include "VKShader.h"
#include "VKUtils.h"
#include "VKDescriptor.h"
#include "VKPipeline.h"

#include <Eklipse/Utils/File.h>
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
		static VkShaderStageFlagBits VKShaderStageFromInternalStage(const ShaderStage stage)
		{
			if (stage == ShaderStage::VERTEX)   return VK_SHADER_STAGE_VERTEX_BIT;
			if (stage == ShaderStage::FRAGMENT) return VK_SHADER_STAGE_FRAGMENT_BIT;

			EK_ASSERT(false, "Unknown shader stage!");
			return VK_SHADER_STAGE_ALL;
		}
		static const char* GetCacheDirectory()
		{
			return "Assets/Cache/Shader/Vulkan";
		}
		static const char* VKShaderStageCachedVulkanFileExtension(const ShaderStage stage)
		{
			switch (stage)
			{
				case ShaderStage::VERTEX:    return ".cached_vulkan.vert";
				case ShaderStage::FRAGMENT:  return ".cached_vulkan.frag";
			}
			EK_ASSERT(false, "Unknown shader stage!");
			return "";
		}
		static VkFormat VertexInputSizeToVKFormat(const size_t size)
		{
			switch (size)
			{
				case sizeof(float):		return VK_FORMAT_R32_SFLOAT;
				case sizeof(glm::vec2): return VK_FORMAT_R32G32_SFLOAT;
				case sizeof(glm::vec3): return VK_FORMAT_R32G32B32_SFLOAT;
				case sizeof(glm::vec4): return VK_FORMAT_R32G32B32A32_SFLOAT;
			}
			EK_ASSERT(false, "Unknown vertex input size!");
			return VK_FORMAT_UNDEFINED;
		}

		void VKShader::CompileOrGetVulkanBinaries(const std::unordered_map<ShaderStage, std::string>& shaderSources)
		{
			shaderc::Compiler compiler;
			shaderc::CompileOptions options;
			options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);
			options.SetOptimizationLevel(shaderc_optimization_level_performance);

			std::filesystem::path cacheDirectory = GetCacheDirectory();

			auto& shaderData = m_vulkanSPIRV;
			shaderData.clear();
			for (auto&& [stage, source] : shaderSources)
			{
				std::filesystem::path shaderFilePath = m_filePath;
				std::filesystem::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() + VKShaderStageCachedVulkanFileExtension(stage));

				std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
				if (in.is_open())
				{
					in.seekg(0, std::ios::end);
					auto size = in.tellg();
					in.seekg(0, std::ios::beg);

					auto& data = shaderData[stage];
					data.resize(size / sizeof(uint32_t));
					in.read((char*)data.data(), size);
				}
				else
				{
					shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, (shaderc_shader_kind)ShaderStageToShaderC(stage), m_filePath.c_str(), options);
					EK_ASSERT(module.GetCompilationStatus() == shaderc_compilation_status_success, "{0}", module.GetErrorMessage());

					shaderData[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());

					std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
					if (out.is_open())
					{
						auto& data = shaderData[stage];
						out.write((char*)data.data(), data.size() * sizeof(uint32_t));
						out.flush();
						out.close();
					}
				}
			}

			Reflect(m_vulkanSPIRV, m_filePath);
		}

		VKShader::VKShader(const std::string& filePath) : m_filePath(filePath)
		{
			CreateCacheDirectoryIfNeeded("Assets/Cache/Shader/Vulkan");

			std::string source = ReadFileFromPath(filePath);
			auto shaderSources = PreProcess(source);

			auto lastSlash = filePath.find_last_of("/\\");
			lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
			auto lastDot = filePath.rfind('.');
			auto count = lastDot == std::string::npos ? filePath.size() - lastSlash : lastDot - lastSlash;
			m_name = filePath.substr(lastSlash, count);

			{
				Timer timer;
				CompileOrGetVulkanBinaries(shaderSources);

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

				std::vector<VkPipelineShaderStageCreateInfo> shaderStages = { vertShaderStageInfo, fragShaderStageInfo };

				// Create descriptor set layout
				bool requiresDescriptorSets = false;
				std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;
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
						requiresDescriptorSets = true;
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
						requiresDescriptorSets = true;
					}
				}

				m_descriptorSetLayout = CreateDescriptorSetLayout(descriptorSetLayoutBindings);

				// Create pipeline layout
				m_pipelineLayout = CreatePipelineLayout({ m_descriptorSetLayout });

				// Create pipeline
				size_t inputSize = 0;
				for (auto& vertexReflection : m_reflections[ShaderStage::VERTEX].inputs)
					inputSize += vertexReflection.size;

				std::vector<VkVertexInputBindingDescription> bindingDescription;
				VkVertexInputBindingDescription vertexInputDescription{};
				vertexInputDescription.binding = 0;
				vertexInputDescription.stride = inputSize;
				vertexInputDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
				bindingDescription.push_back(vertexInputDescription);

				std::vector<VkVertexInputAttributeDescription> attributeDescription;
				for (auto& vertexReflection : m_reflections[ShaderStage::VERTEX].inputs)
				{
					VkVertexInputAttributeDescription attribute{};
					attribute.binding = 0;
					attribute.location = vertexReflection.location;
					attribute.format = VertexInputSizeToVKFormat(vertexReflection.size);
					attribute.offset = vertexReflection.offset;
					attributeDescription.push_back(attribute);
				}

				EK_ASSERT(g_VKSceneFramebuffer != nullptr, "Vulkan Scene Frambuffer is null!");
				m_pipeline = CreateGraphicsPipeline(shaderStages, m_pipelineLayout, g_VKSceneFramebuffer->GetRenderPass(), bindingDescription, attributeDescription);

				vkDestroyShaderModule(g_logicalDevice, fragShaderModule, nullptr);
				vkDestroyShaderModule(g_logicalDevice, vertShaderModule, nullptr);

				EK_CORE_WARN("Creation of shader '{0}' took {1} ms", m_name, timer.ElapsedTimeMs());
			}
		}

		void VKShader::Bind() const 
		{
			vkCmdBindPipeline(g_currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
		}
		void VKShader::Unbind() const {}

		void VKShader::Dispose() const
		{
			vkDestroyPipelineLayout(g_logicalDevice, m_pipelineLayout, nullptr);
			vkDestroyDescriptorSetLayout(g_logicalDevice, m_descriptorSetLayout, nullptr);
			vkDestroyPipeline(g_logicalDevice, m_pipeline, nullptr);
		}
	}
}
