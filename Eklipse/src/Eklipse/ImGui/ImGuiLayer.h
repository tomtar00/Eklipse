#pragma once

#include <imgui.h>
#include <Eklipse/Core/Layer.h>
#include <Eklipse/Core/Window.h>
#include <imgui_internal.h>

namespace Eklipse
{
	class EK_API ImGuiPanel
	{
	public:
		virtual void OnGUI() = 0;
	};

	struct EK_API DockLayoutInfo
	{
		char* name;
		ImGuiDir_ dir;
		float ratio;
	};
	struct EK_API GuiLayerConfigInfo
	{
		bool* enabled;
		bool menuBarEnabled;
		bool dockingEnabled;
		std::vector<DockLayoutInfo> dockLayouts;
		std::vector<ImGuiPanel*> panels;
	};

	class EK_API ImGuiLayer : public Eklipse::Layer
	{
	public:
		ImGuiLayer() = delete;
		ImGuiLayer(Window* window, GuiLayerConfigInfo configInfo) : m_window(window), m_config(configInfo), m_first_time(true) {};
		~ImGuiLayer() {}

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void Update(float deltaTime) override;

	public:
		virtual void Init() = 0;
		virtual void Shutdown() = 0;
		virtual void NewFrame() = 0;
		virtual void Draw(void* data) = 0;

		void AddPanel(ImGuiPanel& panel);
		GuiLayerConfigInfo GetConfig();
		void SetConfig(GuiLayerConfigInfo configInfo);
	
	protected:
		GuiLayerConfigInfo m_config;
		Window* m_window;

	private:
		bool m_first_time;
	};
}