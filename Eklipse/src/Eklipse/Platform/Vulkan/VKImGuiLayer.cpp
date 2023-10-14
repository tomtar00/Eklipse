#include "precompiled.h"
#include "VkImGuiLayer.h"

#include <Eklipse/Renderer/Settings.h>
#include <Eklipse/Renderer/RenderCommand.h>
#include <Eklipse/Platform/Windows/WindowsWindow.h>
#include <backends/imgui_impl_vulkan.h>
#include <backends/imgui_impl_glfw.h>
#include "VK.h"
#include "VKUtils.h"
#include "VKCommands.h"
#include "VKSwapChain.h"
#include "VKPipeline.h"
#include "VKDescriptor.h"
#include <Eklipse/Core/Application.h>

namespace Eklipse
{
	namespace Vulkan
	{
		//VkRenderPass					g_imguiRenderPass = VK_NULL_HANDLE;
		//std::vector<VkCommandBuffer>	g_imguiCommandBuffers{};
		//std::vector<VkFramebuffer>		g_imguiFrameBuffers{};
		//
		VkExtent2D						g_viewportExtent = { 512, 512 };
		//VkRenderPass					g_viewportRenderPass = VK_NULL_HANDLE;
		//VkPipeline						g_viewportPipeline = VK_NULL_HANDLE;
		//VkPipelineLayout				g_viewportPipelineLayout = VK_NULL_HANDLE;
		//std::vector<VkCommandBuffer>	g_viewportCommandBuffers{};
		//std::vector<Image>				g_viewportImages{};
		//std::vector<VkFramebuffer>		g_viewportFrameBuffers{};

		VkImGuiLayer::VkImGuiLayer(const GuiLayerConfigInfo& configInfo) :
			m_imguiPool(VK_NULL_HANDLE), m_imageDescrSets(), Eklipse::ImGuiLayer(configInfo)
		{
			m_glfwWindow = Eklipse::Application::Get().GetWindow()->GetGlfwWindow();
			EK_ASSERT(m_glfwWindow, "Failed to get GLFW window in VK ImGui Layer!");
		}
		void VkImGuiLayer::Init()
		{
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

			//CreateCommandBuffers(g_imguiCommandBuffers, g_maxFramesInFlight, g_commandPool);
			//g_imguiRenderPass = CreateImGuiRenderPass();
			//CreateFrameBuffers(g_imguiFrameBuffers, g_swapChainImageViews, g_imguiRenderPass, g_swapChainExtent, true);
			//
			//CreateCommandBuffers(g_viewportCommandBuffers, g_maxFramesInFlight, g_commandPool);
			//g_viewportRenderPass = CreateViewportRenderPass();

			ImGui_ImplGlfw_InitForVulkan(m_glfwWindow, true);

			ImGui_ImplVulkan_InitInfo init_info = {};
			init_info.Instance = g_instance;
			init_info.PhysicalDevice = g_physicalDevice;
			init_info.Device = g_logicalDevice;
			init_info.Queue = g_graphicsQueue;
			init_info.DescriptorPool = m_imguiPool;
			init_info.MinImageCount = g_swapChainImageCount;
			init_info.ImageCount = g_swapChainImageCount;
			init_info.MSAASamples = (VkSampleCountFlagBits)RendererSettings::GetMsaaSamples();
			init_info.CheckVkResultFn = [](VkResult res) { HANDLE_VK_RESULT(res, "IMGUI") };

			EK_ASSERT(g_VKDefaultFramebuffer != nullptr, "g_framebuffer is null! Maybe forgot to call Renderer::SetGUIFramebuffer()");
			ImGui_ImplVulkan_Init(&init_info, g_VKDefaultFramebuffer->GetRenderPass());

			/*m_imageDescrSets.resize(g_swapChainImageCount);
			for (int i = 0; i < g_swapChainImageCount; i++)
			{
				auto& texture = g_vkViewport->GetFramebuffer()->GetMainColorAttachment(i);
				m_imageDescrSets[i] = ImGui_ImplVulkan_AddTexture(texture.GetSampler(), texture.GetImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
			}*/
			
			//FramebufferInfo framebufferInfo{};
			//framebufferInfo.width = g_viewportSize.width;
			//framebufferInfo.height = g_viewportSize.height;
			//framebufferInfo.numSamples = RendererSettings::GetMsaaSamples();
			//framebufferInfo.colorAttachmentInfos = { { ImageFormat::RGBA8 } };
			//framebufferInfo.depthAttachmentInfo = { ImageFormat::D24S8 };
			//m_framebuffer = CreateRef<VkFramebuffer>(framebufferInfo); // TODO: fix this later

			auto cmd = BeginSingleCommands();
			ImGui_ImplVulkan_CreateFontsTexture(cmd);
			EndSingleCommands(cmd);

			ImGui_ImplVulkan_DestroyFontUploadObjects();

			EK_CORE_INFO("Vulkan ImGui layer initialized");
		}
		void VkImGuiLayer::Shutdown()
		{
			if (!s_initialized) return;
			s_initialized = false;

			// // imgui layer
			// FreeCommandBuffers(g_imguiCommandBuffers, g_commandPool);
			// vkDestroyRenderPass(g_logicalDevice, g_imguiRenderPass, nullptr);
			// DestroyFrameBuffers(g_imguiFrameBuffers);
			// 
			// // viewport
			// FreeCommandBuffers(g_viewportCommandBuffers, g_commandPool);
			// vkDestroyRenderPass(g_logicalDevice, g_viewportRenderPass, nullptr);
			// 
			// vkDestroyPipeline(g_logicalDevice, g_viewportPipeline, nullptr);
			// vkDestroyPipelineLayout(g_logicalDevice, g_viewportPipelineLayout, nullptr);

			vkDestroyDescriptorPool(g_logicalDevice, m_imguiPool, nullptr);

			ImGui_ImplVulkan_Shutdown();
			ImGui_ImplGlfw_Shutdown();

			EK_CORE_INFO("Vulkan ImGui layer shut down");
		}
		void VkImGuiLayer::NewFrame()
		{
			ImGui_ImplVulkan_NewFrame();
			ImGui_ImplGlfw_NewFrame();
		}
		void VkImGuiLayer::Render()
		{
			if (!(*m_config.enabled)) return;

			ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), g_currentCommandBuffer);
		}
		void VkImGuiLayer::DrawViewport(float width, float height)
		{
			if (width != g_viewportExtent.width || height != g_viewportExtent.height)
			{
				ResizeViewport(width, height);
			}

			g_viewportImageIndex = (g_viewportImageIndex + 1) % g_swapChainImageCount;
			//ImGui::Image(m_imageDescrSets[g_viewportImageIndex], ImVec2{ width, height });
		}
		void VkImGuiLayer::ResizeViewport(float width, float height)
		{
			vkDeviceWaitIdle(g_logicalDevice);
			g_viewportExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

			g_VKSceneFramebuffer->Resize(width, height);
		}
		/*void VkImGuiLayer::SetupViewportImages()
		{
			std::vector<VkImageView> views;
			views.resize(g_swapChainImageCount);

			for (int i = 0; i < g_swapChainImageCount; i++)
			{
				g_viewportImages[i].CreateImage(g_viewportExtent.width, g_viewportExtent.height,
					1, (VkSampleCountFlagBits)RendererSettings::GetMsaaSamples(), g_swapChainImageFormat, VK_IMAGE_TILING_OPTIMAL,
					VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);

				g_viewportImages[i].TransitionImageLayout(g_swapChainImageFormat, VK_IMAGE_LAYOUT_UNDEFINED,
					VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1);

				g_viewportImages[i].CreateImageView(g_swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
				g_viewportImages[i].CreateSampler(1);
				views[i] = g_viewportImages[i].m_imageView;

				m_imageDescrSets[i] = ImGui_ImplVulkan_AddTexture(g_viewportImages[i].m_sampler, g_viewportImages[i].m_imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
			}
			CreateFrameBuffers(g_viewportFrameBuffers, views, g_viewportRenderPass, g_viewportExtent, false);
		}
		void VkImGuiLayer::DestroyViewportImages()
		{
			vkFreeDescriptorSets(g_logicalDevice, m_imguiPool, m_imageDescrSets.size(), m_imageDescrSets.data());
			DestroyFrameBuffers(g_viewportFrameBuffers);
			for (auto& image : g_viewportImages)
			{
				image.Dispose();
			}
		}*/
	}
}