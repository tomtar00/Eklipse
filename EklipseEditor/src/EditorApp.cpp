#include <Eklipse.h>
#include <Eklipse/Core/EntryPoint.h>
#include <EklipseScriptAPI.h>

#include "EditorLayer.h"

namespace Eklipse
{
	class EklipseEditor : public Application
	{
	public:
		EklipseEditor(ApplicationInfo& info) : Application(info)
		{
			editorLayer = CreateRef<EditorLayer>();
			PushLayer(editorLayer->GUI);
			PushLayer(editorLayer);
		}	

		void OnAPIHasInitialized(GraphicsAPI::Type api) override
		{
			IMGUI_INIT_FOR_DLL
		}
		void OnShutdownAPI(bool quit) override
		{
			IMGUI_SHUTDOWN_FOR_DLL
		}

		void OnPreGUI(float deltaTime) override
		{
			editorLayer->GUI->Begin();
		}
		void OnPostGUI(float deltaTime) override
		{
			editorLayer->GUI->End();
		}

	private:
		Ref<EditorLayer> editorLayer;
	};
}

Eklipse::Ref<Eklipse::Application> Eklipse::CreateApplication()
{
	Eklipse::ApplicationInfo info{};
	info.appName = "Eklipse Editor";
	info.windowWidth = 600;
	info.windowHeight = 400;

	return Eklipse::CreateRef<Eklipse::EklipseEditor>(info);
}