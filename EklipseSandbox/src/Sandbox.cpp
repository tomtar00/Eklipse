#define EK_PROFILE_NAME(name)	
#define EK_PROFILE()	
#define EK_PROFILE_END_FRAME()

#include <SandboxLayer.h>
#include <Eklipse/Core/EntryPoint.h>

namespace Eklipse
{
    class Sandbox : public Application
    {
    public:
        Sandbox(ApplicationInfo& info) : Application(info)
        {
            m_sandboxLayer = CreateRef<SandboxLayer>();
            PushLayer(m_sandboxLayer);
        }

        void OnAPIHasInitialized(ApiType api) override
        {
            m_sandboxLayer->OnAPIHasInitialized(api);
        }
        void OnShutdownAPI(bool quit) override
        {
            m_sandboxLayer->OnShutdownAPI();
        }

    private:
        Ref<SandboxLayer> m_sandboxLayer;
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