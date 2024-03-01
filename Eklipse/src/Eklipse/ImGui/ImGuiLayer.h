#pragma once

#include <imgui.h>
#include <imgui_internal.h>
#include <Eklipse/Core/Layer.h>
#include <Eklipse/Assets/AssetLibrary.h>
#include <Eklipse/Renderer/Framebuffer.h>
#include <Eklipse/Renderer/GraphicsAPI.h>

#define IMGUI_INIT_FOR_DLL ImGuiLayer::CTX = ImGui::CreateContext();\
                                             ImGui::SetCurrentContext(ImGuiLayer::CTX);

#define IMGUI_SHUTDOWN_FOR_DLL ImGui::SetCurrentContext(ImGuiLayer::CTX = nullptr);

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
        virtual bool OnGUI(float deltaTime);
    public:
        void SetVisible(bool visible);
        bool IsVisible() const;
    private:
        bool m_visible = true;
    };

    enum ImGuiNodeDirType
    {
        Dir_Same = BIT(0),
        Dir_Opposite = BIT(1),
        Dir_Stack = BIT(2),
        Dir_Rest = BIT(3)
    };
    struct DockLayoutInfo
    {
        char* name;
        ImGuiDir_ dir;
        int dirType;
        float ratio;

        ImGuiID id;
    };
    struct ImGuiLayerConfig
    {
        bool menuBarEnabled;
        bool dockingEnabled;
        Vec<DockLayoutInfo> dockLayouts;
        Vec<GuiPanel*> panels;
    };

    class EK_API ImGuiAdapter
    {
    public:
        ImGuiAdapter(const ImGuiLayerConfig& config);

        virtual void Init() = 0;
        virtual void Shutdown() = 0;
        virtual void NewFrame() = 0;
        virtual void Render() = 0;

        virtual void DrawViewport(Framebuffer* framebuffer, float width, float height) = 0;
        virtual void ResizeViewport(Framebuffer* framebuffer, float width, float height) = 0;

        static Ref<ImGuiAdapter> Create(const ImGuiLayerConfig& config);
    };

    class EK_API ImGuiLayer : public Layer
    {
    public:
        ImGuiLayer() = delete;
        ImGuiLayer(const ImGuiLayerConfig& configInfo);
        virtual ~ImGuiLayer() {}

        virtual void OnAttach() override;
        virtual void OnDetach() override;
        virtual void OnGUI(float deltaTime) override;
        virtual void OnRender(float deltaTime) override;

        virtual void OnAPIHasInitialized(GraphicsAPI::Type api) override;
        virtual void OnShutdownAPI(bool quit) override;

        void Begin();
        void DrawDockspace();
        void DrawViewport(Framebuffer* framebuffer, float width, float height);
        void End();
        void Shutdown();

        inline static ImGuiContext* CTX = nullptr;

    private:
        Ref<ImGuiAdapter> m_adapter;
        ImGuiLayerConfig m_config;
        bool m_first_time;
    };
}