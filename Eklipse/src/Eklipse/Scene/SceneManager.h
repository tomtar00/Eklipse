#pragma once
#include "Scene.h"

namespace Eklipse
{
	class SceneManager
	{
	public:
		static void SetActiveScene(const Ref<Scene>& scene);
		static Ref<Scene> GetActiveScene();

	private:
		static Ref<Scene> s_activeScene;
	};
}