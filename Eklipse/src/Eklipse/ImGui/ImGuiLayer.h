#pragma once

#include <imgui.h>
#include <Eklipse/Core/Layer.h>
#include <Eklipse/Core/Window.h>

namespace Eklipse
{
	class ImGuiPanel
	{
	public:
		virtual void OnGUI() = 0;
	};

	class ImGuiLayer : public Eklipse::Layer
	{
	public:
		ImGuiLayer() = delete;
		ImGuiLayer(Window* window) : m_window(window) {};
		~ImGuiLayer() {}

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void Update(float deltaTime) override;

	public:
		virtual void Init() = 0;
		virtual void Shutdown() = 0;
		virtual void NewFrame() = 0;
		void AddPanel(ImGuiPanel& panel);
	
	protected:
		ImGuiIO m_io;
		Window* m_window;
		std::vector<ImGuiPanel*> m_panels;
	};
}