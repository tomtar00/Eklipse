#pragma once
#include <vector>
#include "Entity.h"
#include "Camera.h"

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