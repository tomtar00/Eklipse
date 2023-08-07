#pragma once
#include <Eklipse/Core/Layer.h>
#include <vulkan/vulkan.h>
#include <Eklipse/Platform/Windows/WindowsWindow.h>

namespace Eklipse
{
	class ImGuiLayer : public Eklipse::Layer
	{
	public:
		ImGuiLayer(Window* window) { m_window = dynamic_cast<WindowsWindow*>(window); }
		~ImGuiLayer() {}

		static void Draw(VkCommandBuffer cmd);

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void Update(float deltaTime) override;

		VkDescriptorPool m_imguiPool;
		WindowsWindow* m_window;
	};
}