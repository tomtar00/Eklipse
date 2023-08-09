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
			VkImGuiLayer(Window* window, GuiLayerConfigInfo configInfo);

			virtual void Init() override;
			virtual void Shutdown() override;
			virtual void NewFrame() override;
			virtual void Draw(void* data) override;

		private:
			GLFWwindow* m_glfwWindow;
			VkDescriptorPool m_imguiPool;
		};
	}
}