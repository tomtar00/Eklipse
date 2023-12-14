#include <Eklipse.h>
#include <Eklipse/Core/EntryPoint.h>
#include "EditorLayer.h"
#include <ScriptAPI.h>

namespace Editor
{
	class EklipseEditor : public Eklipse::Application
	{
	public:
		EklipseEditor(Eklipse::ApplicationInfo& info) : Eklipse::Application(info)
		{
			editorLayer = Eklipse::CreateRef<EditorLayer>();
			PushLayer(editorLayer);

			ScriptAPI::ScriptingConfig config{};
			// Logging
			{
				config.loggerConfig.name	= "SCRIPT";
				config.loggerConfig.pattern = "%^[%T] %n: %v%$";
				config.loggerConfig.sink	= GetTerminal()->GetSink();
			}
			ScriptAPI::Init(config);
		}	

		void OnAPIHasInitialized(Eklipse::ApiType api) override
		{
			Eklipse::ImGuiLayer::CTX = ImGui::CreateContext();
			editorLayer->OnAPIHasInitialized(api);
		}
		void OnShutdownAPI() override
		{
			editorLayer->OnShutdownAPI();
		}

		void OnPreGUI(float deltaTime) override
		{
			EK_PROFILE();

			editorLayer->GUI->Begin();
			editorLayer->GUI->DrawDockspace();
		}
		void OnPostGUI(float deltaTime) override
		{
			EK_PROFILE();

			editorLayer->GUI->End();
		}
		void OnPreUpdate(float deltaTime) override
		{
			EK_PROFILE();
		}
		void OnPostUpdate(float deltaTime) override
		{
			EK_PROFILE();

			editorLayer->Render(m_scene, deltaTime);
		}

	private:
		Eklipse::Ref<EditorLayer> editorLayer;
	};
}

Eklipse::Ref<Eklipse::Application> Eklipse::CreateApplication()
{
	Eklipse::ApplicationInfo info{};
	info.appName = "Eklipse Editor";
	info.windowWidth = 1600;
	info.windowHeight = 900;

	return Eklipse::CreateRef<Editor::EklipseEditor>(info);
}