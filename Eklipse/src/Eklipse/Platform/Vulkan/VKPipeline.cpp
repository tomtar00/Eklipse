#include "precompiled.h"
#include "VK.h"

#include "VKPipeline.h"
#include "VKUtils.h"
#include "VKDescriptor.h"
#include "VKShader.h"

#include <Eklipse/Utils/File.h>
#include <Eklipse/Renderer/Renderer.h>

namespace Eklipse
{
    namespace Vulkan
    {        
        VkPipelineLayout CreatePipelineLayout(Vec<VkDescriptorSetLayout> descSetLayouts, Vec<VkPushConstantRange> pushConstantRanges)
        {
            EK_CORE_PROFILE();
            VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
            pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descSetLayouts.size());
            pipelineLayoutInfo.pSetLayouts = descSetLayouts.data();
            pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size());
            pipelineLayoutInfo.pPushConstantRanges = pushConstantRanges.data();

            VkPipelineLayout pipelineLayout;
            VkResult res = vkCreatePipelineLayout(g_logicalDevice, &pipelineLayoutInfo, nullptr, &pipelineLayout);
            HANDLE_VK_RESULT(res, "CREATE PIPELINE LAYOUT");

            return pipelineLayout;
        }

        VkPipeline CreateGraphicsPipeline(const GraphicsPipelineCreateInfo& createInfo)
        {
            EK_CORE_PROFILE();
            VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
            vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(createInfo.bindingDesc.size());
            vertexInputInfo.pVertexBindingDescriptions = createInfo.bindingDesc.data();
            vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(createInfo.attribteDesc.size());
            vertexInputInfo.pVertexAttributeDescriptions = createInfo.attribteDesc.data();

            VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
            inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            inputAssembly.topology = createInfo.topology;
            inputAssembly.primitiveRestartEnable = VK_FALSE;

            VkPipelineViewportStateCreateInfo viewportState{};
            viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            viewportState.viewportCount = 1;
            viewportState.scissorCount = 1;

            VkPipelineRasterizationStateCreateInfo rasterizer{};
            rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            rasterizer.depthClampEnable = VK_FALSE;
            rasterizer.rasterizerDiscardEnable = VK_FALSE;
            rasterizer.polygonMode = createInfo.mode;
            rasterizer.lineWidth = 1.0f;
            rasterizer.cullMode = VK_CULL_MODE_FRONT_BIT;
            rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
            rasterizer.depthBiasEnable = VK_FALSE;
            rasterizer.depthBiasConstantFactor = 0.0f;
            rasterizer.depthBiasClamp = 0.0f;
            rasterizer.depthBiasSlopeFactor = 0.0f;

            VkPipelineMultisampleStateCreateInfo multisampling{};
            multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            multisampling.sampleShadingEnable = VK_TRUE;
            multisampling.rasterizationSamples = (VkSampleCountFlagBits)Renderer::GetSettings().GetMsaaSamples();
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

            Vec<VkDynamicState> dynamicStates = {
                VK_DYNAMIC_STATE_VIEWPORT,
                VK_DYNAMIC_STATE_SCISSOR
            };

            VkPipelineDynamicStateCreateInfo dynamicState{};
            dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
            dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
            dynamicState.pDynamicStates = dynamicStates.data();

            VkGraphicsPipelineCreateInfo pipelineInfo{};
            pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            pipelineInfo.layout = createInfo.pipelineLayout;
            pipelineInfo.renderPass = createInfo.renderPass;
            pipelineInfo.subpass = 0;
            pipelineInfo.stageCount = static_cast<uint32_t>(createInfo.shaderStages.size());
            pipelineInfo.pStages = createInfo.shaderStages.data();
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

            VkPipeline pipeline;
            VkResult res = vkCreateGraphicsPipelines(g_logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline);
            HANDLE_VK_RESULT(res, "CREATE GRAPHICS PIPELINE");

            return pipeline;
        }
        VkPipeline CreateComputePipeline(const ComputePipelineCreateInfo& createInfo)
        {
            EK_CORE_PROFILE();
            /*auto& computeShaderCode = Eklipse::ReadFileFromPath(shaderRelPath);
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

            VkPipeline pipeline;
            res = vkCreateComputePipelines(g_logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline);
            HANDLE_VK_RESULT(res, "CREATE COMPUTE PIPELINES");

            vkDestroyShaderModule(g_logicalDevice, computeShaderModule, nullptr);

            return pipeline;*/

            return VK_NULL_HANDLE;
        }
        
        VKPipeline::VKPipeline(const Pipeline::Config& config) : Pipeline(config)
        {
            EK_CORE_PROFILE();
            Build();
        }

        void VKPipeline::Build()
        {
            EK_CORE_TRACE("Building pipeline");
            if (m_config.type == Pipeline::Type::Resterization)
            {
                VKShader* shader = static_cast<VKShader*>(m_config.shader);
                VKFramebuffer* framebuffer = static_cast<VKFramebuffer*>(m_config.framebuffer);

                GraphicsPipelineCreateInfo info{};
                info.shaderStages = CreateShaderStages(shader->GetVertexShaderModule(), shader->GetFragmentShaderModule());
                info.attribteDesc = CreateVertexInputAttributeDescriptions(shader->GetVertexReflection());
                info.bindingDesc = CreateVertexInputBindingDescriptions(shader->GetVertexReflection());
                info.pipelineLayout = shader->GetPipelineLayout();
                info.renderPass = framebuffer->GetRenderPass();
                if (m_config.mode == Pipeline::Mode::Triangle)
                {
                    info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
                    info.mode = VK_POLYGON_MODE_FILL;
                }
                else if (m_config.mode == Pipeline::Mode::Line)
                {
                    info.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
                    info.mode = VK_POLYGON_MODE_LINE;
                }
                m_pipeline = CreateGraphicsPipeline(info);
                m_bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            }
            /*
            else if (config.type == Pipeline::Type::RayTracing)
            {
                 m_bindPoint = VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR;
            }
            */
            /*
            else if (config.type == Pipeline::Type::Compute)
            {
                Ref<VKShader> shader = Cast<VKShader>(config.shader);
                Ref<VKFramebuffer> framebuffer = Cast<VKFramebuffer>(config.framebuffer);

                m_pipeline = CreateComputePipeline(shader->GetPath().c_str(), shader->GetPipelineLayout(), nullptr);

                 m_bindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;
            }
            */
            else
            {
                EK_CORE_ERROR("PIPELINE TYPE NOT SUPPORTED: {}", (int)m_config.type);
            }
        }
        void VKPipeline::Bind()
        {
            EK_CORE_PROFILE();
            vkCmdBindPipeline(g_currentCommandBuffer, m_bindPoint, m_pipeline);
        }
        void VKPipeline::Dispose()
        {
            EK_CORE_PROFILE();
            EK_CORE_TRACE("Disposing pipeline");
            vkDestroyPipeline(g_logicalDevice, m_pipeline, nullptr);
            EK_CORE_DBG("Pipeline disposed");
        }
    }
}