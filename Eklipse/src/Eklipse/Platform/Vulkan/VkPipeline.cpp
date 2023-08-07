#include "precompiled.h"
#include "_globals.h"

#include "VkPipeline.h"
#include "VkUtils.h"
#include "VkDescriptor.h"

#include <Eklipse/Utils/File.h>
#include <Eklipse/Renderer/Settings.h>

namespace Eklipse
{
	namespace Vulkan
	{
        VkRenderPass        g_renderPass                = VK_NULL_HANDLE;
        VkPipeline          g_graphicsPipeline          = VK_NULL_HANDLE;
        VkPipelineLayout    g_graphicsPipelineLayout    = VK_NULL_HANDLE;
        //VkPipeline          g_particlePipeline          = VK_NULL_HANDLE;
        //VkPipelineLayout    g_particlePipelineLayout    = VK_NULL_HANDLE;
        //VkPipeline          g_computePipeline           = VK_NULL_HANDLE;
        //VkPipelineLayout    g_computePipelineLayout     = VK_NULL_HANDLE;

		void CreateGraphicsPipeline(const char* vertShaderRelPath, const char* fragShaderRelPath, 
			VkPipelineLayout& pipelineLayout, VkPipeline& pipeline, VkRenderPass& renderPass, 
			std::vector<VkVertexInputBindingDescription> vertBindingDesc, 
			std::vector<VkVertexInputAttributeDescription> vertAttribteDesc,
            VkDescriptorSetLayout* descSetLayouts)
		{
            auto vertShaderCode = Eklipse::ReadFileFromPath(vertShaderRelPath);
            VkShaderModule vertShaderModule = CreateShaderModule(vertShaderCode);
            VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
            vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
            vertShaderStageInfo.module = vertShaderModule;
            vertShaderStageInfo.pName = "main";

            auto fragShaderCode = Eklipse::ReadFileFromPath(fragShaderRelPath);
            VkShaderModule fragShaderModule = CreateShaderModule(fragShaderCode);
            VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
            fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            fragShaderStageInfo.module = fragShaderModule;
            fragShaderStageInfo.pName = "main";

            VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

            VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
            vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            vertexInputInfo.vertexBindingDescriptionCount = vertBindingDesc.size();
            vertexInputInfo.pVertexBindingDescriptions = vertBindingDesc.data();
            vertexInputInfo.vertexAttributeDescriptionCount = vertAttribteDesc.size();
            vertexInputInfo.pVertexAttributeDescriptions = vertAttribteDesc.data();

            VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
            inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            inputAssembly.primitiveRestartEnable = VK_FALSE;

            VkPipelineViewportStateCreateInfo viewportState{};
            viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            viewportState.viewportCount = 1;
            viewportState.scissorCount = 1;

            VkPipelineRasterizationStateCreateInfo rasterizer{};
            rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            rasterizer.depthClampEnable = VK_FALSE;
            rasterizer.rasterizerDiscardEnable = VK_FALSE;
            rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
            rasterizer.lineWidth = 1.0f;
            rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
            rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
            rasterizer.depthBiasEnable = VK_FALSE;
            rasterizer.depthBiasConstantFactor = 0.0f;
            rasterizer.depthBiasClamp = 0.0f;
            rasterizer.depthBiasSlopeFactor = 0.0f;

            VkPipelineMultisampleStateCreateInfo multisampling{};
            multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            multisampling.sampleShadingEnable = VK_TRUE;
            multisampling.rasterizationSamples = RendererSettings::msaaSamples;
            multisampling.minSampleShading = 0.2f;
            multisampling.pSampleMask = nullptr;
            multisampling.alphaToCoverageEnable = VK_FALSE;
            multisampling.alphaToOneEnable = VK_FALSE;

            VkPipelineColorBlendAttachmentState colorBlendAttachment{};
            colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
            colorBlendAttachment.blendEnable = VK_FALSE;
            colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
            colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
            colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
            colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
            colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
            colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

            VkPipelineColorBlendStateCreateInfo colorBlending{};
            colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            colorBlending.logicOpEnable = VK_FALSE;
            colorBlending.logicOp = VK_LOGIC_OP_COPY;
            colorBlending.attachmentCount = 1;
            colorBlending.pAttachments = &colorBlendAttachment;
            colorBlending.blendConstants[0] = 0.0f;
            colorBlending.blendConstants[1] = 0.0f;
            colorBlending.blendConstants[2] = 0.0f;
            colorBlending.blendConstants[3] = 0.0f;

            VkPipelineDepthStencilStateCreateInfo depthStencil{};
            depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
            depthStencil.depthTestEnable = VK_TRUE;
            depthStencil.depthWriteEnable = VK_TRUE;
            depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
            depthStencil.depthBoundsTestEnable = VK_FALSE;
            depthStencil.stencilTestEnable = VK_FALSE;

            VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
            pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipelineLayoutInfo.setLayoutCount = 1;
            pipelineLayoutInfo.pSetLayouts = descSetLayouts;
            pipelineLayoutInfo.pushConstantRangeCount = 0;
            pipelineLayoutInfo.pPushConstantRanges = nullptr;

            std::vector<VkDynamicState> dynamicStates = {
                VK_DYNAMIC_STATE_VIEWPORT,
                VK_DYNAMIC_STATE_SCISSOR
            };

            VkPipelineDynamicStateCreateInfo dynamicState{};
            dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
            dynamicState.dynamicStateCount = dynamicStates.size();
            dynamicState.pDynamicStates = dynamicStates.data();

            VkResult res;
            res = vkCreatePipelineLayout(g_logicalDevice, &pipelineLayoutInfo, nullptr, &pipelineLayout);
            HANDLE_VK_RESULT(res, "CREATE PIPELINE LAYOUT");

            VkGraphicsPipelineCreateInfo pipelineInfo{};
            pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            pipelineInfo.stageCount = 2;
            pipelineInfo.layout = pipelineLayout;
            pipelineInfo.renderPass = renderPass;
            pipelineInfo.subpass = 0;
            pipelineInfo.pStages = shaderStages;
            pipelineInfo.pVertexInputState = &vertexInputInfo;
            pipelineInfo.pInputAssemblyState = &inputAssembly;
            pipelineInfo.pViewportState = &viewportState;
            pipelineInfo.pRasterizationState = &rasterizer;
            pipelineInfo.pMultisampleState = &multisampling;
            pipelineInfo.pDepthStencilState = nullptr;
            pipelineInfo.pColorBlendState = &colorBlending;
            pipelineInfo.pDynamicState = &dynamicState;
            pipelineInfo.pDepthStencilState = &depthStencil;
            pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
            pipelineInfo.basePipelineIndex = -1;

            res = vkCreateGraphicsPipelines(g_logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline);
            HANDLE_VK_RESULT(res, "CREATE GRAPHICS PIPELINES");

            vkDestroyShaderModule(g_logicalDevice, fragShaderModule, nullptr);
            vkDestroyShaderModule(g_logicalDevice, vertShaderModule, nullptr);
		}
        void CreateComputePipeline(const char* shaderRelPath, VkPipelineLayout& pipelineLayout,
            VkPipeline& pipeline, VkDescriptorSetLayout* descSetLayout)
        {
            auto computeShaderCode = Eklipse::ReadFileFromPath(shaderRelPath);
            VkShaderModule computeShaderModule = CreateShaderModule(computeShaderCode);
            VkPipelineShaderStageCreateInfo computeShaderStageInfo{};
            computeShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            computeShaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
            computeShaderStageInfo.module = computeShaderModule;
            computeShaderStageInfo.pName = "main";

            VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
            pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipelineLayoutInfo.setLayoutCount = 1;
            pipelineLayoutInfo.pSetLayouts = descSetLayout;

            VkResult res;
            res = vkCreatePipelineLayout(g_logicalDevice, &pipelineLayoutInfo, nullptr, &pipelineLayout);
            HANDLE_VK_RESULT(res, "CREATE COMPUTE PIPELINE LAYOUT");

            VkComputePipelineCreateInfo pipelineInfo{};
            pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
            pipelineInfo.layout = pipelineLayout;
            pipelineInfo.stage = computeShaderStageInfo;

            res = vkCreateComputePipelines(g_logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline);
            HANDLE_VK_RESULT(res, "CREATE COMPUTE PIPELINES");

            vkDestroyShaderModule(g_logicalDevice, computeShaderModule, nullptr);
        }
        void CreateRenderPass(VkRenderPass& renderPass)
        {
            VkAttachmentDescription colorAttachment{};
            colorAttachment.format = g_swapChainImageFormat;
            colorAttachment.samples = (VkSampleCountFlagBits)RendererSettings::msaaSamples;
            colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            VkAttachmentReference colorAttachmentRef{};
            colorAttachmentRef.attachment = 0;
            colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            VkAttachmentDescription depthAttachment{};
            depthAttachment.format = FindDepthFormat();
            depthAttachment.samples = (VkSampleCountFlagBits)RendererSettings::msaaSamples;
            depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            VkAttachmentReference depthAttachmentRef{};
            depthAttachmentRef.attachment = 1;
            depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            VkAttachmentDescription colorAttachmentResolve{};
            colorAttachmentResolve.format = g_swapChainImageFormat;
            colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
            colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

            VkAttachmentReference colorAttachmentResolveRef{};
            colorAttachmentResolveRef.attachment = 2;
            colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            VkSubpassDescription subpass{};
            subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpass.colorAttachmentCount = 1;
            subpass.pColorAttachments = &colorAttachmentRef;
            subpass.pDepthStencilAttachment = &depthAttachmentRef;
            subpass.pResolveAttachments = &colorAttachmentResolveRef;

            std::array<VkAttachmentDescription, 3> attachments =
            {
                colorAttachment,
                depthAttachment,
                colorAttachmentResolve
            };

            VkRenderPassCreateInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
            renderPassInfo.attachmentCount = attachments.size();
            renderPassInfo.pAttachments = attachments.data();
            renderPassInfo.subpassCount = 1;
            renderPassInfo.pSubpasses = &subpass;

            VkSubpassDependency dependency{};
            dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
            dependency.dstSubpass = 0;
            dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            dependency.srcAccessMask = 0;
            dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

            renderPassInfo.dependencyCount = 1;
            renderPassInfo.pDependencies = &dependency;

            VkResult res = vkCreateRenderPass(g_logicalDevice, &renderPassInfo, nullptr, &renderPass);
            HANDLE_VK_RESULT(res, "CREATE RENDER PASS");
        }
        
        void SetupPipelines()
        {
            CreateRenderPass(g_renderPass);

            // Graphics pipelines
            {
                CreateGraphicsPipeline("shaders/vert.spv", "shaders/frag.spv",
                    g_graphicsPipelineLayout, g_graphicsPipeline, g_renderPass,
                    GetVertexBindingDescription(), GetVertexAttributeDescriptions(),
                    &g_graphicsDescriptorSetLayout);

                // CreateGraphicsPipeline("shaders/particle-vert.spv", "shaders/particle-frag.spv",
                //     g_particlePipelineLayout, g_particlePipeline, g_renderPass,
                //     GetParticleBindingDescription(), GetParticleAttributeDescriptions(),
                //     &g_graphicsDescriptorSetLayout);
            }

            // Compute pipelines
            // {
            //     CreateComputePipeline("shaders/particle-comp.spv", g_computePipelineLayout, 
            //         g_computePipeline, &g_computeDescriptorSetLayout);
            // }
        }
        void DisposePipelines()
        {
            vkDestroyRenderPass(g_logicalDevice, g_renderPass, nullptr);

            vkDestroyPipeline(g_logicalDevice, g_graphicsPipeline, nullptr);
            vkDestroyPipelineLayout(g_logicalDevice, g_graphicsPipelineLayout, nullptr);

            //vkDestroyPipeline(g_logicalDevice, g_particlePipeline, nullptr);
            //vkDestroyPipelineLayout(g_logicalDevice, g_particlePipelineLayout, nullptr);
            //
            //vkDestroyPipeline(g_logicalDevice, g_computePipeline, nullptr);
            //vkDestroyPipelineLayout(g_logicalDevice, g_computePipelineLayout, nullptr);
        }
	}
}