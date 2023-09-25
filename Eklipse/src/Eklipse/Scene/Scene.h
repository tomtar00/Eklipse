#pragma once
#include <vector>
#include "Entity.h"
#include "Camera.h"

#include <entt.hpp>
#include <glm/glm.hpp>

namespace Eklipse
{
	class Scene
	{
	public:
		void Load();
		void Dispose();

		Entity CreateEntity(const std::string name = "");
		entt::registry& GetRegistry() { return m_registry; }

	private:
		entt::registry m_registry;
	};
}