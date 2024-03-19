#include "precompiled.h"
#include "SceneManager.h"

#include <Eklipse/Renderer/Renderer.h>

namespace Eklipse
{
	Ref<Scene> SceneManager::s_activeScene = nullptr;

	void SceneManager::SetActiveScene(Ref<Scene> scene)
	{
		scene->Dispose();
		s_activeScene = scene;
		s_activeScene->ApplyAllComponents();
		Renderer::OnSceneChanged();
	}
	Ref<Scene> SceneManager::GetActiveScene()
	{
		return s_activeScene;
	}
	void SceneManager::Dispose()
    {
		EK_CORE_PROFILE();
        s_activeScene.reset();
		s_activeScene = nullptr;
    }
}