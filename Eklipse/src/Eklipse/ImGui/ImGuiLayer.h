#pragma once

#include <imgui.h>
#include <imgui_internal.h>
#include <Eklipse/Core/Layer.h>

namespace Eklipse
{
	enum EK_API ImGuiNodeDirType
	{
		Dir_Same		= BIT(0),
		Dir_Opposite	= BIT(1),
		Dir_Stack		= BIT(2)
	};

	struct EK_API DockLayoutInfo
	{
		char* name;
		ImGuiDir_ dir;
		int dirType;
		float ratio;
		Ref<Layer> layer;

		ImGuiID id;
	};
	struct EK_API GuiLayerConfigInfo
	{
		bool* enabled;
		bool menuBarEnabled;
		bool dockingEnabled;
		std::vector<DockLayoutInfo> dockLayouts;
	};

	class EK_API ImGuiLayer : public Eklipse::Layer
	{
	public:
		ImGuiLayer() = delete;
		ImGuiLayer(const GuiLayerConfigInfo& configInfo);
		virtual ~ImGuiLayer() {}

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void Begin();
		void RenderDockspace();
		void End();

	public:
		virtual void Init() = 0;
		virtual void Shutdown() = 0;
		virtual void NewFrame() = 0;
		virtual void Draw() = 0;

		virtual void DrawViewport(float width, float height) = 0;
		virtual void ResizeViewport(float width, float height) = 0;

		GuiLayerConfigInfo GetConfig();
		void SetConfig(GuiLayerConfigInfo configInfo);
	
		static Ref<ImGuiLayer> Create(const GuiLayerConfigInfo& configInfo);

		inline bool IsEnabled() const { return *m_config.enabled; }
		inline static ImGuiContext* s_ctx = nullptr;

	protected:		
		inline static bool s_initialized = false;

		GuiLayerConfigInfo m_config;

	private:
		bool m_first_time;
	};
}