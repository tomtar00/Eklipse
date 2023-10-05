#include "EditorLayer.h"
#include <Eklipse/Core/EntryPoint.h>

namespace Editor
{
	class EklipseEditor : public Eklipse::Application
	{
	public:
		EklipseEditor(Eklipse::ApplicationInfo& info) : Application(info)
		{
			Eklipse::ImGuiLayer::s_ctx = ImGui::CreateContext();

			editorLayer = Eklipse::CreateRef<EditorLayer>(m_scene);
			PushLayer(editorLayer);

			editorLayer->GUI = Eklipse::ImGuiLayer::Create(editorLayer->GetGuiInfo());
			PushOverlay(editorLayer->GUI);
		}

		void Run() override
		{
			EK_INFO("========== Starting Eklipse Editor ==========");

			Application::Init();

			float deltaTime = 0.0f;
			while (Application::IsRunning())
			{
				Application::BeginFrame(&deltaTime);
				if (Application::IsMinimized()) continue;
				
				{
					EK_PROFILE_NAME("GUI");

					editorLayer->GUI->Begin();
					editorLayer->GUI->RenderDockspace();
					for (auto& layer : m_layerStack)
					{
						layer->OnGUI(deltaTime);
					}
					editorLayer->GUI->End();
				}

				for (auto& layer : m_layerStack)
				{
					layer->OnUpdate(deltaTime);
				}

				editorLayer->Render(m_scene, deltaTime);
				
				Application::EndFrame(deltaTime);	
			}

			EK_INFO("========== Closing Eklipse Editor ==========");

			Application::Shutdown();
		}	

		void OnInitAPI(Eklipse::ApiType api) override
		{
			editorLayer->GUI->Init();
		}
		void OnShutdownAPI() override
		{
			editorLayer->GUI->Shutdown();
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