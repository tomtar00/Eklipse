#pragma once

#include <imgui.h>
#include <imgui_internal.h>
#include <Eklipse/Core/Layer.h>
#include <Eklipse/Assets/AssetLibrary.h>
#include <Eklipse/Renderer/Framebuffer.h>

#define EK_COLOR_WINDOW_BG ImVec4(0.09f, 0.09f, 0.09f, 1.00f)

#define EK_COLOR_HEADER ImVec4(0.20f, 0.20f, 0.20f, 1.00f)
#define EK_COLOR_HEADER_HOVERED ImVec4(0.30f, 0.30f, 0.30f, 1.00f)
#define EK_COLOR_HEADER_ACTIVE ImVec4(0.12f, 0.12f, 0.12f, 1.00f)

#define EK_COLOR_BUTTON ImVec4(0.20f, 0.20f, 0.20f, 1.00f)
#define EK_COLOR_BUTTON_HOVERED ImVec4(0.30f, 0.30f, 0.30f, 1.00f)
#define EK_COLOR_BUTTON_ACTIVE ImVec4(0.12f, 0.12f, 0.12f, 1.00f)

#define EK_COLOR_FRAME_BG ImVec4(0.20f, 0.20f, 0.20f, 1.00f)
#define EK_COLOR_FRAME_BG_HOVERED ImVec4(0.30f, 0.30f, 0.30f, 1.00f)
#define EK_COLOR_FRAME_BG_ACTIVE ImVec4(0.12f, 0.12f, 0.12f, 1.00f)

#define EK_COLOR_TAB ImVec4(0.12f, 0.12f, 0.12f, 1.00f)
#define EK_COLOR_TAB_HOVERED ImVec4(0.30f, 0.30f, 0.30f, 1.00f)
#define EK_COLOR_TAB_ACTIVE ImVec4(0.12f, 0.12f, 0.12f, 1.00f)
#define EK_COLOR_TAB_UNFOCUSED ImVec4(0.12f, 0.12f, 0.12f, 1.00f)
#define EK_COLOR_TAB_UNFOCUSED_ACTIVE ImVec4(0.12f, 0.12f, 0.12f, 1.00f)

#define EK_COLOR_TITLE_BG ImVec4(0.20f, 0.20f, 0.20f, 1.00f)
#define EK_COLOR_TITLE_BG_ACTIVE ImVec4(0.12f, 0.12f, 0.12f, 1.00f)
#define EK_COLOR_TITLE_BG_COLLAPSED ImVec4(0.20f, 0.20f, 0.20f, 1.00f)

namespace Eklipse
{
    class EK_API GuiIcon
    {
    public:
        static Ref<GuiIcon> Create(const Path& texturePath);
        virtual void* GetID() = 0;
        virtual void Dispose() = 0;
    };
    class EK_API GuiPanel
    {
    public:
        GuiPanel();
        virtual bool OnGUI(float deltaTime) { return IsVisible(); };
    public:
        void SetVisible(bool visible) { m_visible = visible; }
        bool IsVisible() const { return m_visible; }
    private:
        bool m_visible = true;
    };

    enum ImGuiNodeDirType
    {
        Dir_Same		= BIT(0),
        Dir_Opposite	= BIT(1),
        Dir_Stack		= BIT(2),
        Dir_Rest		= BIT(3)
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
        Vec<DockLayoutInfo> dockLayouts;
        Vec<GuiPanel*> panels;
    };

    class EK_API ImGuiLayer : public Layer
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

        virtual void DrawViewport(Framebuffer* framebuffer, float width, float height) = 0;
        virtual void ResizeViewport(Framebuffer* framebuffer, float width, float height) = 0;

        GuiLayerConfigInfo GetConfig();
        void SetConfig(GuiLayerConfigInfo configInfo);

        void SetupColors();
    
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