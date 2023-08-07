#pragma once
#include "Model.h"
#include "Camera.h"
#include <vector>

namespace Eklipse
{
	class Scene
	{
	public:
		void Load();
		void Dispose();

		Camera m_camera;
		std::vector<Model> m_geometry;
	};
}