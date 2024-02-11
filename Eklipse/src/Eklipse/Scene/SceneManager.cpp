#include "precompiled.h"
#include "SceneManager.h"

namespace Eklipse
{
	Ref<Scene> SceneManager::s_activeScene = nullptr;

	void SceneManager::SetActiveScene(Ref<Scene> scene)
	{
		s_activeScene = scene;
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