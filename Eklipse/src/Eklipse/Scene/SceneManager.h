#pragma once
#include "Scene.h"

namespace Eklipse
{
	class EK_API SceneManager
	{
	public:
		static void SetActiveScene(const Ref<Scene>& scene);
		static Ref<Scene> GetActiveScene();

	private:
		static Ref<Scene> s_activeScene;
	};
}