#pragma once

#include <vulkan/vulkan.h>
#include <Eklipse/ImGui/ImGuiLayer.h>
#include <GLFW/glfw3.h>

namespace Eklipse
{
	namespace Vulkan
	{
		class VkImGuiLayer : public Eklipse::ImGuiLayer
		{
		public:
			VkImGuiLayer(const GuiLayerConfigInfo& configInfo);
			virtual ~VkImGuiLayer() {}

			virtual void Init() override;
			virtual void Shutdown() override;
			virtual void NewFrame() override;
			virtual void Draw() override;

			virtual void DrawViewport(float width, float height) override;
			virtual void ResizeViewport(float width, float height) override;

		private:
			GLFWwindow* m_glfwWindow;
			VkDescriptorPool m_imguiPool;

			std::vector<VkDescriptorSet> m_imageDescrSets;
		};
	}
}