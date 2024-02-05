#pragma once
#include "Scene.h"

namespace Eklipse
{
	class EK_API SceneManager
	{
	public:
		static void SetActiveScene(Ref<Scene> scene);
		static Ref<Scene> GetActiveScene();

		static void Dispose();

	private:
		static Ref<Scene> s_activeScene;
	};
}