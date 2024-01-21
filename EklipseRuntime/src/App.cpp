#include <Eklipse.h>
#include <Eklipse/Core/EntryPoint.h>
#include "AppLayer.h"
#include <EklipseScriptAPI.h>

namespace Eklipse
{
	class App : public Application
	{
	public:
		App(ApplicationInfo& info) : Application(info)
		{
			m_appLayer = CreateRef<AppLayer>();

			EklipseScriptAPI::ScriptingConfig config{};
			// Logging
			{
				config.loggerConfig.name = "SCRIPT";
				config.loggerConfig.pattern = "%^[%T] %n: %v%$";
				config.loggerConfig.sink = GetTerminal()->GetSink();
			}
			EklipseScriptAPI::Init(config);

			PushLayer(m_appLayer);
		}

		void OnAPIHasInitialized(ApiType api) override
		{
			m_appLayer->OnAPIHasInitialized(api);
		}
		void OnShutdownAPI() override
		{
			m_appLayer->OnShutdownAPI();
		}

	private:
		Ref<AppLayer> m_appLayer;
	};
}

Eklipse::Ref<Eklipse::Application> Eklipse::CreateApplication()
{
	Eklipse::ApplicationInfo info{};
	info.appName = "Eklipse App";
	info.windowWidth = 1600;
	info.windowHeight = 900;

	return Eklipse::CreateRef<Eklipse::App>(info);
}