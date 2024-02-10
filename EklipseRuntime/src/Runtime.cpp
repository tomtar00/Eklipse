#define EK_PROFILE_NAME(name)	
#define EK_PROFILE()			

#include <Eklipse.h>
#include <Eklipse/Core/EntryPoint.h>
#include "RuntimeLayer.h"
#include <EklipseScriptAPI.h>

namespace Eklipse
{
	class Runtime : public Application
	{
	public:
		Runtime(ApplicationInfo& info) : Application(info)
		{
			m_runtimeLayer = CreateRef<RuntimeLayer>();

			EklipseScriptAPI::ScriptingConfig config{};
			// Logging
			{
				config.loggerConfig.name = "SCRIPT";
				config.loggerConfig.pattern = "%^[%T] %n: %v%$";
				config.loggerConfig.sink = Log::clientLogger()->sinks()[0];
			}
			EklipseScriptAPI::Init(config);

			PushLayer(m_runtimeLayer);
		}

		void OnAPIHasInitialized(ApiType api) override
		{
			m_runtimeLayer->OnAPIHasInitialized(api);
		}
		void OnShutdownAPI(bool quit) override
		{
			m_runtimeLayer->OnShutdownAPI();
		}

	private:
		Ref<RuntimeLayer> m_runtimeLayer;
	};
}

Eklipse::Ref<Eklipse::Application> Eklipse::CreateApplication()
{
	Eklipse::ApplicationInfo info{};
	info.appName = "Eklipse App";
	info.windowWidth = 1600;
	info.windowHeight = 900;

	return Eklipse::CreateRef<Eklipse::Runtime>(info);
}