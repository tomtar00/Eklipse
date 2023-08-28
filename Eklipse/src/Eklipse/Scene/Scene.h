#pragma once
#include "Camera.h"
#include <vector>
#include "Entity.h"

namespace Eklipse
{
	class Scene
	{
	public:
		void Load();
		void Dispose();

		Camera m_camera;
		std::vector<Entity> m_entities;
	};
}