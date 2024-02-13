#include "precompiled.h"
#include "VkImGuiLayer.h"

#include "VK.h"
#include "VKUtils.h"
#include "VKCommands.h"
#include "VKSwapChain.h"
#include "VKPipeline.h"
#include "VKDescriptor.h"

#include <Eklipse/Renderer/RenderCommand.h>
#include <Eklipse/Platform/Windows/WindowsWindow.h>
#include <Eklipse/Core/Application.h>

#include <backends/imgui_impl_vulkan.h>
#include <backends/imgui_impl_glfw.h>

#include <Eklipse/Assets/AssetManager.h>

namespace Eklipse
{
    namespace Vulkan
    {
        VkExtent2D	g_viewportExtent = { 512, 512 };

        VkImGuiLayer::VkImGuiLayer(const GuiLayerConfigInfo& configInfo) :
            m_imguiPool(VK_NULL_HANDLE), m_imageDescrSets(), Eklipse::ImGuiLayer(configInfo)
        {
            EK_CORE_PROFILE();
            m_glfwWindow = Eklipse::Application::Get().GetWindow()->GetGlfwWindow();
            EK_ASSERT(m_glfwWindow, "Failed to get GLFW window in VK ImGui Layer!");
        }
        void VkImGuiLayer::Init()
        {
            EK_CORE_PROFILE();
            if (s_initialized) return;
            s_initialized = true;

            m_imguiPool = CreateDescriptorPool({
                { VK_DESCRIPTOR_TYPE_SAMPLER,					1000 },
                { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,	1000 },
                { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,				1000 },
                { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,				1000 },
                { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,		1000 },
                { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,		1000 },
                { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,			1000 },
                { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,			1000 },
                { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,	1000 },
                { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,	1000 },
                { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,			1000 }
            }, 100, VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT);

            ImGui_ImplGlfw_InitForVulkan(m_glfwWindow, true);

            ImGui_ImplVulkan_InitInfo init_info = {};
            init_info.Instance = g_instance;
            init_info.PhysicalDevice = g_physicalDevice;
            init_info.Device = g_logicalDevice;
            init_info.Queue = g_graphicsQueue;
            init_info.DescriptorPool = m_imguiPool;
            init_info.MinImageCount = g_swapChainImageCount;
            init_info.ImageCount = g_swapChainImageCount;
            init_info.MSAASamples = (VkSampleCountFlagBits)Renderer::GetSettings().GetMsaaSamples();
            init_info.CheckVkResultFn = [](VkResult res) { HANDLE_VK_RESULT(res, "IMGUI") };

            EK_ASSERT(g_VKDefaultFramebuffer != nullptr, "Default framebuffer is null!");
            ImGui_ImplVulkan_Init(&init_info, g_VKDefaultFramebuffer->GetRenderPass());

            //auto cmd = BeginSingleCommands();
            ImGui_ImplVulkan_CreateFontsTexture();
            //EndSingleCommands(cmd);

            //ImGui_ImplVulkan_DestroyFontUploadObjects();
        }
        void VkImGuiLayer::Shutdown()
        {
            if (!s_initialized) return;
            s_initialized = false;

            vkFreeDescriptorSets(g_logicalDevice, m_imguiPool, m_imageDescrSets.size(), m_imageDescrSets.data());
            vkDestroyDescriptorPool(g_logicalDevice, m_imguiPool, nullptr);

            ImGui_ImplVulkan_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGuiLayer::Shutdown();
        }
        void VkImGuiLayer::NewFrame()
        {
            EK_CORE_PROFILE();
            ImGui_ImplVulkan_NewFrame();
            ImGui_ImplGlfw_NewFrame();
        }
        void VkImGuiLayer::Render()
        {
            EK_CORE_PROFILE();
            if (!(*m_config.enabled)) return;

            ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), g_currentCommandBuffer);
        }
        void VkImGuiLayer::DrawViewport(Framebuffer* framebuffer, float width, float height)
        {
            EK_CORE_PROFILE();
            if (m_imageDescrSets.size() <= 0) // TODO: What if we want to draw multiple viewports?
            {
                SetupDescriptorSets(framebuffer);
            }

            VKFramebuffer* vkFramebuffer = static_cast<VKFramebuffer*>(framebuffer);
            uint32_t* vkImageIndex = vkFramebuffer->GetImageIndexPtr();
            *vkImageIndex = (*vkImageIndex + 1) % g_swapChainImageCount;

            if (width != framebuffer->GetInfo().width || height != framebuffer->GetInfo().height)
            {
                ResizeViewport(framebuffer, width, height);
            }

            ImGui::Image(m_imageDescrSets[*vkImageIndex], ImVec2{ width, height });

            /*for (int i = 0; i < g_swapChainImageCount; ++i)
            {
                ImGui::Image(m_imageDescrSets[i], ImVec2{ width, height / g_swapChainImageCount });
            }*/
        }
        void VkImGuiLayer::ResizeViewport(Framebuffer* framebuffer, float width, float height)
        {
            EK_CORE_PROFILE();
            if (m_imageDescrSets.size() <= 0) // TODO: What if we want to draw multiple viewports?
            {
                SetupDescriptorSets(framebuffer);
            }

            if (width > 0 && height > 0)
            {
                vkDeviceWaitIdle(g_logicalDevice);
                g_viewportExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

                framebuffer->Resize(g_viewportExtent.width, g_viewportExtent.height);

                vkFreeDescriptorSets(g_logicalDevice, m_imguiPool, m_imageDescrSets.size(), m_imageDescrSets.data());
                SetupDescriptorSets(framebuffer);
            }
        }
        void VkImGuiLayer::SetupDescriptorSets(Framebuffer* framebuffer)
        {
            EK_CORE_PROFILE();
            VKFramebuffer* vkFramebuffer = static_cast<VKFramebuffer*>(framebuffer);
            m_imageDescrSets.resize(g_swapChainImageCount);
            for (int i = 0; i < g_swapChainImageCount; ++i)
            {
                auto& texture = vkFramebuffer->GetMainColorAttachment(i);
                m_imageDescrSets[i] = ImGui_ImplVulkan_AddTexture(texture.GetSampler(), texture.GetImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            }
        }

        // ==================== ICONS ===================

        VKImGuiIcon::VKImGuiIcon(const Path& texturePath)
        {
            EK_CORE_PROFILE();
            m_texture = CreateRef<VKTexture2D>(texturePath);
            m_descriptorSet = ImGui_ImplVulkan_AddTexture(m_texture->GetSampler(), m_texture->GetImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        }
        void* VKImGuiIcon::GetID()
        {
            return m_descriptorSet;
        }
        void VKImGuiIcon::Dispose()
        {
            EK_CORE_PROFILE();
            m_texture->Dispose();
        }
}
}