#include "EditorLayer.h"
#include <Eklipse/Core/EntryPoint.h>

#define EK_EDITOR

namespace Editor
{
	class EklipseEditor : public Eklipse::Application
	{
	public:
		EklipseEditor(Eklipse::ApplicationInfo& info) : Application(info)
		{
			Eklipse::ImGuiLayer::s_ctx = ImGui::CreateContext();

			editorLayer = Eklipse::CreateRef<EditorLayer>();
			PushLayer(editorLayer);

			SetGuiLayer(editorLayer->GetGuiInfo());
		}

	private:
		Eklipse::Ref<EditorLayer> editorLayer;
	};
}

Eklipse::Ref<Eklipse::Application> Eklipse::CreateApplication()
{
	EK_INFO("Starting editor...");

	Eklipse::ApplicationInfo info{};
	info.appName = "Eklipse Editor";
	info.windowWidth = 1600;
	info.windowHeight = 900;

	return Eklipse::CreateRef<Editor::EklipseEditor>(info);
}