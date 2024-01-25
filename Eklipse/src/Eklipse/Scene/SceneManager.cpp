#include "precompiled.h"
#include "SceneManager.h"

namespace Eklipse
{
	Ref<Scene> SceneManager::s_activeScene = nullptr;
	void SceneManager::SetActiveScene(const Ref<Scene>& scene)
	{
		s_activeScene = scene;
	}
	Ref<Scene> SceneManager::GetActiveScene()
	{
		return s_activeScene;
	}
}
