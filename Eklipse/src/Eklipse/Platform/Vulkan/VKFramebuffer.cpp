#include "precompiled.h"
#include "VKFramebuffer.h"

#include "VK.h"
#include "VKCommands.h"
#include "VKSwapChain.h"

namespace Eklipse
{
    namespace Vulkan
    {
        VKFramebuffer::VKFramebuffer(const FramebufferInfo& framebufferInfo) 
            : Framebuffer(framebufferInfo), m_framebufferInfo(framebufferInfo)
        {
            EK_CORE_PROFILE();
            if (framebufferInfo.isDefaultFramebuffer)
            {
                EK_ASSERT(g_VKDefaultFramebuffer == nullptr, "Default framebuffer already exists!");
                g_defaultFramebuffer = g_VKDefaultFramebuffer = this;
            }
            else
            {
                g_offScreenFramebuffers.push_back(this);
                g_VKOffScreenFramebuffers.push_back(this);
            }

            CreateCommandBuffers(m_commandBuffers, g_maxFramesInFlight, g_commandPool);
            Build();
        }

        void VKFramebuffer::DestroyFramebuffers()
        {
            EK_CORE_PROFILE();

            vkDeviceWaitIdle(g_logicalDevice);
            vkDestroyRenderPass(g_logicalDevice, m_renderPass, nullptr);

            if (m_framebufferInfo.isDefaultFramebuffer)
            {
                DestroyImageViews(g_swapChainImageViews);
                vkDestroySwapchainKHR(g_logicalDevice, g_swapChain, nullptr);
            }

            for (uint32_t i = 0; i < g_swapChainImageCount; i++)
            {
                for (auto& colorAttachment : m_framebufferAttachments[i].colorAttachments)
                    colorAttachment->Dispose();
                if (m_framebufferAttachments[i].depthAttachment)
                    m_framebufferAttachments[i].depthAttachment->Dispose();

                vkDestroyFramebuffer(g_logicalDevice, m_framebuffers[i], nullptr);
            }
        }

        VKTexture2D& VKFramebuffer::GetMainColorAttachment(uint32_t index)
        {
            return *m_framebufferAttachments[index].colorAttachments[0];
        }
        VkRenderPass VKFramebuffer::GetRenderPass() const
        {
            return m_renderPass;
        }
        VkCommandBuffer VKFramebuffer::GetCommandBuffer(uint32_t index)
        {
            return m_commandBuffers[index];
        }
        uint32_t* VKFramebuffer::GetImageIndexPtr()
        {
            return &m_imageIndex;
        }

        FramebufferInfo& VKFramebuffer::GetInfo()
        {
            return m_framebufferInfo;
        }
        void VKFramebuffer::Build()
        {	
            EK_CORE_PROFILE();

            m_framebuffers.resize(g_swapChainImageCount);
            m_framebufferAttachments.resize(g_swapChainImageCount);

            if (m_framebufferInfo.isDefaultFramebuffer)
            {
                if (m_framebufferInfo.colorAttachmentInfos.size() > 1)
                {
                    EK_CORE_WARN("Eklipse only supports 1 color attachment for default framebuffer!");
                }
                VkFormat desiredFormat = ConvertToVKFormat(m_framebufferInfo.colorAttachmentInfos[0].textureFormat);
                g_swapChainImageFormat = desiredFormat;
                g_swapChain = CreateSwapChain(m_framebufferInfo.width, m_framebufferInfo.height,
                    g_swapChainImageCount, g_swapChainImageFormat, g_swapChainExtent, g_swapChainImages);
                if (g_swapChainImageFormat != desiredFormat)
                {
                    EK_CORE_DBG("Desired format not supported by swap chain! Using {0} instead", (int)g_swapChainImageFormat);
                }
                CreateImageViews(g_swapChainImageViews, g_swapChainImages, g_swapChainImageFormat);

                m_renderPass = CreateRenderPass();
                m_framebuffers.resize(g_swapChainImageCount);
                m_framebufferAttachments.resize(g_swapChainImageCount);
                for (size_t i = 0; i < g_swapChainImageViews.size(); ++i)
                {
                    Vec<VkImageView> attachments;

                    attachments.push_back(g_swapChainImageViews[i]);

                    auto& depthAttachmentInfo = m_framebufferInfo.depthAttachmentInfo;
                    if (depthAttachmentInfo.textureFormat != ImageFormat::FORMAT_UNDEFINED)
                    {
                        TextureInfo textureInfo{};
                        textureInfo.width = m_framebufferInfo.width;
                        textureInfo.height = m_framebufferInfo.height;
                        textureInfo.mipMapLevel = 1;
                        textureInfo.samples = m_framebufferInfo.numSamples;
                        textureInfo.imageFormat = depthAttachmentInfo.textureFormat;
                        textureInfo.imageAspect = ImageAspect::DEPTH;
                        textureInfo.imageUsage = ImageUsage::DEPTH_ATTACHMENT;

                        Ref<VKTexture2D> texture = std::static_pointer_cast<VKTexture2D>(Texture2D::Create(textureInfo));
                        m_framebufferAttachments[i].depthAttachment = texture;
                        attachments.push_back(texture->GetImageView());
                    }

                    VkFramebufferCreateInfo framebufferInfo{};
                    framebufferInfo.sType			= VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
                    framebufferInfo.renderPass		= m_renderPass;
                    framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
                    framebufferInfo.pAttachments	= attachments.data();
                    framebufferInfo.width			= m_framebufferInfo.width;
                    framebufferInfo.height			= m_framebufferInfo.height;
                    framebufferInfo.layers			= 1;

                    VkResult res = vkCreateFramebuffer(g_logicalDevice, &framebufferInfo, nullptr, &m_framebuffers[i]);
                    HANDLE_VK_RESULT(res, "CREATE FRAMEBUFFER");
                }
                return;
            }

            m_renderPass = CreateRenderPass();
            m_framebuffers.resize(g_swapChainImageCount);
            m_framebufferAttachments.resize(g_swapChainImageCount);
            for (uint32_t idx = 0; idx < g_swapChainImageCount; idx++)
            {
                Vec<VkImageView> attachments;

                // Color attachments
                m_framebufferAttachments[idx].colorAttachments.resize(m_framebufferInfo.colorAttachmentInfos.size());
                for (size_t i = 0; i < m_framebufferInfo.colorAttachmentInfos.size(); i++)
                {
                    auto& colorAttachmentInfo = m_framebufferInfo.colorAttachmentInfos[i];

                    TextureInfo textureInfo{};
                    textureInfo.width		= m_framebufferInfo.width;
                    textureInfo.height		= m_framebufferInfo.height;
                    textureInfo.mipMapLevel = 1;
                    textureInfo.samples		= m_framebufferInfo.numSamples;
                    textureInfo.imageFormat = colorAttachmentInfo.textureFormat;
                    textureInfo.imageLayout = ImageLayout::SHADER_READ_ONLY;
                    textureInfo.imageAspect = ImageAspect::COLOR;
                    textureInfo.imageUsage	= ImageUsage::COLOR_ATTACHMENT | ImageUsage::SAMPLED;

                    Ref<VKTexture2D> texture = std::static_pointer_cast<VKTexture2D>(Texture2D::Create(textureInfo));
                    m_framebufferAttachments[idx].colorAttachments[i] = texture;
                    attachments.push_back(texture->GetImageView());
                }

                // Depth attachment
                auto& depthAttachmentInfo = m_framebufferInfo.depthAttachmentInfo;
                if (depthAttachmentInfo.textureFormat != ImageFormat::FORMAT_UNDEFINED) 
                {
                    TextureInfo textureInfo{};
                    textureInfo.width		= m_framebufferInfo.width;
                    textureInfo.height		= m_framebufferInfo.height;
                    textureInfo.mipMapLevel	= 1;
                    textureInfo.samples		= m_framebufferInfo.numSamples;
                    textureInfo.imageFormat	= depthAttachmentInfo.textureFormat;
                    textureInfo.imageAspect	= ImageAspect::DEPTH;
                    textureInfo.imageUsage	= ImageUsage::DEPTH_ATTACHMENT;

                    Ref<VKTexture2D> texture = std::static_pointer_cast<VKTexture2D>(Texture2D::Create(textureInfo));
                    m_framebufferAttachments[idx].depthAttachment = texture;
                    attachments.push_back(texture->GetImageView());
                }

                VkFramebufferCreateInfo framebufferInfo{};
                framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
                framebufferInfo.renderPass = m_renderPass;
                framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
                framebufferInfo.pAttachments = attachments.data();
                framebufferInfo.width = m_framebufferInfo.width;
                framebufferInfo.height = m_framebufferInfo.height;
                framebufferInfo.layers = 1;

                VkResult res = vkCreateFramebuffer(g_logicalDevice, &framebufferInfo, nullptr, &m_framebuffers[idx]);
                HANDLE_VK_RESULT(res, "CREATE FRAMEBUFFER");
            }
        }
        void VKFramebuffer::Bind()
        {
            EK_CORE_PROFILE();

            g_currentCommandBuffer = m_commandBuffers[g_currentFrame];
            vkResetCommandBuffer(g_currentCommandBuffer, 0);

            VkExtent2D extent = { m_framebufferInfo.width, m_framebufferInfo.height };
            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

            VkResult res = vkBeginCommandBuffer(g_currentCommandBuffer, &beginInfo);
            HANDLE_VK_RESULT(res, "BEGIN RENDER PASS COMMAND BUFFER");

            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = m_renderPass;
            renderPassInfo.framebuffer = m_framebuffers[m_imageIndex];
            renderPassInfo.renderArea.offset = { 0, 0 };
            renderPassInfo.renderArea.extent = extent;

            std::array<VkClearValue, 2> clearValues{};
            clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
            clearValues[1].depthStencil = { 1.0f, 0 };

            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();

            vkCmdBeginRenderPass(g_currentCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            /*VkViewport viewport{};
            viewport.x = 0.0f;
            viewport.y = static_cast<float>(m_framebufferInfo.height);
            viewport.width = static_cast<float>(m_framebufferInfo.width);
            viewport.height = -static_cast<float>(m_framebufferInfo.height);
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;*/

            VkViewport viewport{};
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = static_cast<float>(m_framebufferInfo.width);
            viewport.height = static_cast<float>(m_framebufferInfo.height);
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;

            vkCmdSetViewport(g_currentCommandBuffer, 0, 1, &viewport);
            
            VkRect2D scissor{};
            scissor.offset = { 0, 0 };
            scissor.extent = extent;
            vkCmdSetScissor(g_currentCommandBuffer, 0, 1, &scissor);

            g_currentFramebuffer = this;
        }
        void VKFramebuffer::Unbind()
        {
            EK_CORE_PROFILE();

            vkCmdEndRenderPass(g_currentCommandBuffer);
            VkResult res = vkEndCommandBuffer(g_currentCommandBuffer);
            HANDLE_VK_RESULT(res, "END COMMAND BUFFER");

            g_currentFramebuffer = nullptr;
        }
        void VKFramebuffer::Resize(uint32_t width, uint32_t height)
        {
            EK_CORE_PROFILE();

            Framebuffer::Resize(width, height);

            // destroy
            DestroyFramebuffers();

            // create
            m_framebufferInfo.width = width;
            m_framebufferInfo.height = height;
            Build();
        }
        void VKFramebuffer::Dispose()
        {
            EK_CORE_PROFILE();
            DestroyFramebuffers();
            FreeCommandBuffers(m_commandBuffers, g_commandPool);
        }

        VkRenderPass VKFramebuffer::CreateRenderPass()
        {
            EK_CORE_PROFILE();

            size_t size = m_framebufferInfo.colorAttachmentInfos.size();
            bool hasDepthAttachment = m_framebufferInfo.depthAttachmentInfo.textureFormat != ImageFormat::FORMAT_UNDEFINED;

            Vec<VkAttachmentDescription> attachments;
            if (hasDepthAttachment)
                attachments.resize(size + 1);
            else
                attachments.resize(size);

            Vec<VkAttachmentReference> colorAttachmentRefs;
            colorAttachmentRefs.resize(size);

            for (size_t i = 0; i < size; i++)
            {
                auto& colorAttachmentInfo = m_framebufferInfo.colorAttachmentInfos[i];

                attachments[i].samples = (VkSampleCountFlagBits)m_framebufferInfo.numSamples;
                attachments[i].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                attachments[i].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                attachments[i].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                attachments[i].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                attachments[i].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

                if (m_framebufferInfo.isDefaultFramebuffer)
                {
                    attachments[i].format = g_swapChainImageFormat;
                    attachments[i].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
                }
                else
                {
                    attachments[i].format = ConvertToVKFormat(colorAttachmentInfo.textureFormat);
                    attachments[i].finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                }

                colorAttachmentRefs[i].attachment = i;
                colorAttachmentRefs[i].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            }

            VkAttachmentReference depthAttachmentRef{};
            if (hasDepthAttachment)
            {
                auto& depthAttachmentInfo = m_framebufferInfo.depthAttachmentInfo;
                attachments[size].format = ConvertToVKFormat(depthAttachmentInfo.textureFormat);
                attachments[size].samples = (VkSampleCountFlagBits)m_framebufferInfo.numSamples;
                attachments[size].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                attachments[size].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                attachments[size].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                attachments[size].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                attachments[size].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                attachments[size].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

                depthAttachmentRef.attachment = 1;
                depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            }

            VkSubpassDescription subpass{};
            subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpass.colorAttachmentCount = static_cast<uint32_t>(colorAttachmentRefs.size());
            subpass.pColorAttachments = colorAttachmentRefs.data();
            subpass.pDepthStencilAttachment = hasDepthAttachment ? &depthAttachmentRef : VK_NULL_HANDLE;

            if (m_framebufferInfo.numSamples > 1)
            {
                VkAttachmentDescription colorAttachmentResolve{};
                colorAttachmentResolve.format = g_swapChainImageFormat;
                colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
                colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

                VkAttachmentReference colorAttachmentResolveRef{};
                colorAttachmentResolveRef.attachment = 2;
                colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

                attachments.push_back(colorAttachmentResolve);

                subpass.pResolveAttachments = &colorAttachmentResolveRef;
            }

            Vec<VkSubpassDependency> dependencies;
            {
                VkSubpassDependency dependency{};
                dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
                dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                dependency.srcAccessMask = 0;
                dependency.dstSubpass = 0;
                dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

                if (hasDepthAttachment)
                {
                    dependency.srcStageMask |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
                    dependency.dstStageMask |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
                    dependency.dstAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                }

                dependencies.push_back(dependency);
            }

            VkRenderPassCreateInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
            renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            renderPassInfo.pAttachments = attachments.data();
            renderPassInfo.subpassCount = 1;
            renderPassInfo.pSubpasses = &subpass;
            renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
            renderPassInfo.pDependencies = dependencies.data();

            VkRenderPass renderPass;
            VkResult res = vkCreateRenderPass(g_logicalDevice, &renderPassInfo, nullptr, &renderPass);
            HANDLE_VK_RESULT(res, "CREATE RENDER PASS");

            return renderPass;
        }
    }
}