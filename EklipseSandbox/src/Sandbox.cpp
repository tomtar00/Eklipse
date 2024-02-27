#define EK_PROFILE_NAME(name)
#define EK_PROFILE()	
#define EK_PROFILE_END_FRAME()

#include <SandboxLayer.h>
#include <RTLayer.h>

#include <Eklipse/Core/EntryPoint.h>

namespace Eklipse
{
    class Sandbox : public Application
    {
    public:
        Sandbox(ApplicationInfo& info) : Application(info)
        {
            PushLayer(CreateRef<SandboxLayer>());
            //PushLayer(CreateRef<RTLayer>());

            ImGuiLayerConfig config{};
            config.dockingEnabled = false;
            config.menuBarEnabled = false;
            GUI = CreateRef<ImGuiLayer>(config);
            PushOverlay(GUI);
        }

        void OnAPIHasInitialized(ApiType api) override
        {
            IMGUI_INIT_FOR_DLL
        }
        void OnShutdownAPI(bool quit) override
        {
            IMGUI_SHUTDOWN_FOR_DLL
        }

        void OnPreGUI(float deltaTime) override
        {
            GUI->Begin();
        }
        void OnPostGUI(float deltaTime) override
        {
            GUI->End();
        }

    private:
        Ref<ImGuiLayer> GUI;
    };
}

Eklipse::Ref<Eklipse::Application> Eklipse::CreateApplication()
{
    Eklipse::ApplicationInfo info{};
    info.appName = "Eklipse Sandbox";
    info.windowWidth = 1600;
    info.windowHeight = 900;

    return Eklipse::CreateRef<Eklipse::Sandbox>(info);
}