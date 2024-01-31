#include "precompiled.h"
#include "SceneManager.h"

namespace Eklipse
{
	Scene* SceneManager::s_activeScene = nullptr;

	void SceneManager::SetActiveScene(Scene* scene)
	{
		s_activeScene = scene;
	}
	Scene* SceneManager::GetActiveScene()
	{
		return s_activeScene;
	}
}