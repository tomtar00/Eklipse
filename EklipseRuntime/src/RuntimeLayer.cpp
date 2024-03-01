#include "RuntimeLayer.h"
#include <Eklipse/Project/ProjectSerializer.h>

#define EK_RUNTIME_CONFIG "config.yaml"

namespace Eklipse
{
	bool started = false;

	void RuntimeLayer::OnAttach()
	{
		m_runtimeConfig = CreateRef<RuntimeConfig>();
		ProjectSerializer serializer;
		if (!serializer.DeserializeRuntimeConfig(*m_runtimeConfig, EK_RUNTIME_CONFIG))
		{
			EK_CRITICAL("Failed to deserialize runtime config! Make sure that the '{}' file is in the same folder as the executable file", EK_RUNTIME_CONFIG);
			exit(-1);
		}
		Project::SetRuntimeConfig(m_runtimeConfig);

		// Load script library
		auto& scriptLibraryPath = m_runtimeConfig->scriptsLibraryPath;
		if (FileUtilities::IsPathValid(scriptLibraryPath))
		{
			m_scriptLinker = CreateRef<ScriptLinker>();
			m_scriptLinker->LinkScriptLibrary(scriptLibraryPath);
		}
		else
		{
			EK_INFO("Scripts library not found. No scripts will be attached");
		}

		// Load assets
		m_runtimeAssetLibrary = CreateRef<RuntimeAssetLibrary>(m_runtimeConfig->assetsDirectoryPath);

		// Load start scene
		auto scene = AssetManager::GetAsset<Scene>(m_runtimeConfig->startSceneHandle);
		SceneManager::SetActiveScene(scene);
	}
	void RuntimeLayer::OnDetach()
	{
		ProjectSerializer serializer;
		if (!serializer.SerializeRuntimeConfig(*m_runtimeConfig, EK_RUNTIME_CONFIG))
		{
			EK_ERROR("Failed to serialize runtime config!");
		}
	}

	void RuntimeLayer::OnUpdate(float deltaTime)
	{
		SceneManager::GetActiveScene()->OnSceneUpdate(deltaTime);

		Renderer::BeginDefaultRenderPass();
		Renderer::RenderScene(SceneManager::GetActiveScene());
		Renderer::EndDefaultRenderPass();
	}
	void RuntimeLayer::OnGUI(float deltaTime)
	{
		// TODO: Terminal Panel (ImGui)
	}

	void RuntimeLayer::OnAPIHasInitialized(GraphicsAPI api)
	{
		if (!started)
		{
			// Start scene
			SceneManager::GetActiveScene()->OnSceneStart();
			started = true;
		}

		// Set window title
		Application::Get().GetWindow()->SetTitle(m_runtimeConfig->name.c_str());

		m_runtimeAssetLibrary->ReloadAssets();
		SceneManager::GetActiveScene()->ApplyAllComponents();
	}
	void RuntimeLayer::OnShutdownAPI()
	{
        m_runtimeAssetLibrary->UnloadAssets();
	}
}