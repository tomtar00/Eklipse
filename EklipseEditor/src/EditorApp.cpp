#include "EditorLayer.h"
#include <Eklipse/Core/EntryPoint.h>

namespace Editor
{
	class EklipseEditor : public Eklipse::Application
	{
	public:
		EklipseEditor(Eklipse::ApplicationInfo& info) : Application(info)
		{
			editorLayer = Eklipse::CreateRef<EditorLayer>(m_scene);
			PushLayer(editorLayer);
		}

		void Run() override
		{
			EK_INFO("========== Starting Eklipse Editor ==========");

			Eklipse::ImGuiLayer::CTX = ImGui::CreateContext();
			Application::Init();

			float deltaTime = 0.0f;
			while (Application::IsRunning())
			{
				Application::BeginFrame(&deltaTime);

				if (!Application::IsMinimized())
				{
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

					{
						EK_PROFILE_NAME("Update");

						for (auto& layer : m_layerStack)
						{
							layer->OnUpdate(deltaTime);
						}
					}

					editorLayer->Render(m_scene, deltaTime);
				}

				Application::EndFrame(deltaTime);	
			}

			EK_INFO("========== Closing Eklipse Editor ==========");

			Application::Shutdown();
		}	

		void OnAPIHasInitialized(Eklipse::ApiType api) override
		{
			editorLayer->OnAPIHasInitialized(api);
		}
		void OnShutdownAPI() override
		{
			editorLayer->OnShutdownAPI();
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