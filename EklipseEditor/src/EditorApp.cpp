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
			PushLayer(editorLayer);
		}	

		void OnAPIHasInitialized(ApiType api) override
		{
			ImGuiLayer::CTX = ImGui::CreateContext();
			ImGui::SetCurrentContext(ImGuiLayer::CTX);
			editorLayer->OnAPIHasInitialized(api);
		}
		void OnShutdownAPI(bool quit) override
		{
			editorLayer->OnShutdownAPI(quit);
			ImGui::SetCurrentContext(ImGuiLayer::CTX = nullptr);
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

	private:
		Ref<EditorLayer> editorLayer;
	};
}

Eklipse::Ref<Eklipse::Application> Eklipse::CreateApplication()
{
	Eklipse::ApplicationInfo info{};
	info.appName = "Eklipse Editor";
	info.windowWidth = 1600;
	info.windowHeight = 900;

	return Eklipse::CreateRef<Eklipse::EklipseEditor>(info);
}