#pragma once

#include <vulkan/vulkan.h>
#include <Eklipse/ImGui/ImGuiLayer.h>
#include <GLFW/glfw3.h>
#include "VKTexture.h"

namespace Eklipse
{
	namespace Vulkan
	{
		class VKImGuiIcon : public GuiIcon
		{
		public:
			VKImGuiIcon(const Ref<AssetLibrary> assetLibrary, const Path& path);
			virtual void* GetID() override;

		private:
			Ref<VKTexture2D> m_texture;
			VkDescriptorSet m_descriptorSet;
		};

		class VkImGuiLayer : public Eklipse::ImGuiLayer
		{
		public:
			VkImGuiLayer(const GuiLayerConfigInfo& configInfo);
			virtual ~VkImGuiLayer() {}

			virtual void Init() override;
			virtual void Shutdown() override;
			virtual void NewFrame() override;
			virtual void Render() override;

			virtual void DrawViewport(Framebuffer* framebuffer, float width, float height) override;
			virtual void ResizeViewport(Framebuffer* framebuffer, float width, float height) override;

		private:
			void SetupDescriptorSets(Framebuffer* framebuffer);

		private:
			GLFWwindow* m_glfwWindow;
			VkDescriptorPool m_imguiPool;

			std::vector<VkDescriptorSet> m_imageDescrSets;
		};
	}
}