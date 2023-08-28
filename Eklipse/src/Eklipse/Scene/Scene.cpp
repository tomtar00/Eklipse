#include "precompiled.h"
#include "Scene.h"

namespace Eklipse
{
	void Scene::Load()
	{
		EK_CORE_TRACE("Begin scene load");

		m_camera.m_fov = 65.0f;
		m_camera.m_transform.position = { 1.1f, 1.3f, 2.1f };
		m_camera.m_transform.rotation = { 0, 261.0f, 0 };
		m_camera.m_transform.scale = { 1, 1, 1 };

		for (int i = -1; i <= 1; i++)
		{
			Entity entity{"Entity " + i};
			Mesh mesh{"models/viking_room.obj", "textures/viking_room.png" };
			entity.m_mesh = mesh;
			//entity.m_components.push_back(mesh);
			
			entity.m_transform.position = { i, i, i };
			entity.m_transform.rotation = { i * 90, i * 180, i * 90 };
			entity.m_transform.scale = { 1, 1, 1 };

			m_entities.push_back(entity);
		}

		EK_CORE_TRACE("Scene loaded");
	}
	void Scene::Dispose()
	{
		// TODO: dispose something...
	}
}