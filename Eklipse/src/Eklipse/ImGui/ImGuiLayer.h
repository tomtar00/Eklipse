#pragma once

#include <imgui.h>
#include <imgui_internal.h>
#include <Eklipse/Core/Layer.h>
#include <Eklipse/Core/Window.h>

namespace Eklipse
{
	class EK_API ImGuiPanel
	{
	public:
		virtual void OnGUI() = 0;
	};

	enum EK_API ImGuiNodeDirType
	{
		Dir_Same		= BIT(0),
		Dir_Opposite	= BIT(1)
	};

	struct EK_API DockLayoutInfo
	{
		char* name;
		ImGuiDir_ dir;
		int dirType;
		float ratio;

		ImGuiID id;
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
		ImGuiLayer(Window* window, GuiLayerConfigInfo configInfo);
		virtual ~ImGuiLayer() {}

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void Update(float deltaTime) override;

	public:
		virtual void Init() = 0;
		virtual void Shutdown() = 0;
		virtual void NewFrame() = 0;
		virtual void Draw() = 0;

		virtual void DrawViewport(float width, float height) = 0;
		virtual void ResizeViewport(float width, float height) = 0;

		void AddPanel(ImGuiPanel& panel);
		GuiLayerConfigInfo GetConfig();
		void SetConfig(GuiLayerConfigInfo configInfo);
	
		static Ref<ImGuiLayer> Create(Window* window, const GuiLayerConfigInfo& configInfo);

		inline static ImGuiContext* s_ctx = nullptr;

	protected:		
		inline static bool s_initialized = false;

		GuiLayerConfigInfo m_config;
		Window* m_window;

	private:
		bool m_first_time;
	};
}