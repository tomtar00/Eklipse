#pragma once

#include <imgui.h>
#include <imgui_internal.h>
#include "ImGuiExtensions.h"
#include <Eklipse/Core/Layer.h>
#include <Eklipse/Assets/AssetLibrary.h>

namespace Eklipse
{
	class GuiIcon
	{
	public:
		virtual void* GetID() = 0;
		static Ref<GuiIcon> Create(const Ref<AssetLibrary> assetLibrary, const Path& path);
	};
	class GuiPanel
	{
	public:
		virtual void OnGUI(float deltaTime) = 0;
	};

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

		ImGuiID id;
	};
	struct EK_API GuiLayerConfigInfo
	{
		bool* enabled;
		bool menuBarEnabled;
		bool dockingEnabled;
		std::vector<DockLayoutInfo> dockLayouts;
		std::vector<GuiPanel*> panels;
	};

	class EK_API ImGuiLayer : public Eklipse::Layer
	{
	public:
		ImGuiLayer() = delete;
		ImGuiLayer(const GuiLayerConfigInfo& configInfo);
		virtual ~ImGuiLayer() {}

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnGUI(float deltaTime) override;

		void Begin();
		void DrawDockspace();
		void End();

	public:
		virtual void Init() = 0;
		virtual void Shutdown();
		virtual void NewFrame() = 0;
		virtual void Render() = 0;

		virtual void DrawViewport(float width, float height) = 0;
		virtual void ResizeViewport(float width, float height) = 0;

		GuiLayerConfigInfo GetConfig();
		void SetConfig(GuiLayerConfigInfo configInfo);
	
		static Ref<ImGuiLayer> Create(const GuiLayerConfigInfo& configInfo);

		inline bool IsEnabled() const { return *m_config.enabled; }
		inline static ImGuiContext* CTX = nullptr;

	protected:		
		inline static bool s_initialized = false;
		GuiLayerConfigInfo m_config;

	private:
		bool m_first_time;
	};
}