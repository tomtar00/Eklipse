#pragma once
#include "Scene.h"

namespace Eklipse
{
	class EK_API SceneManager
	{
	public:
		static void SetActiveScene(Scene* scene);
		static Scene* GetActiveScene();

	private:
		static Scene* s_activeScene;
	};
}