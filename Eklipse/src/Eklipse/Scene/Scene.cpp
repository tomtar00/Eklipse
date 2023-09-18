#include "precompiled.h"
#include "Scene.h"

namespace Eklipse
{
	void Scene::Load()
	{
		EK_CORE_TRACE("Begin scene load");

		m_camera.m_name = "Main Camera";
		m_camera.m_fov = 65.0f;
		m_camera.m_transform.position = { 0.0f,  0.5f,  2.0f };
		m_camera.m_transform.rotation = { 0.0f,  0.0f,  0.0f };
		m_camera.m_transform.scale =	{ 1.0f,  1.0f,  1.0f };

		for (int i = -1; i <= 1; i++)
		{
			Mesh mesh{"models/viking_room.obj", "textures/viking_room.png" };
			std::string entityName = "Entity " + std::to_string(i+1);
			Entity entity{entityName, mesh};
			//entity.m_components.push_back(mesh);
			
			entity.m_transform.position =	{ i*1.5f,	0.0f,  0.0f };
			entity.m_transform.rotation =	{ 90.0f,	90.0f, 0.0f };
			entity.m_transform.scale =		{ 1.0f,		1.0f,  1.0f };

			m_entities.push_back(entity);
		}

		EK_CORE_TRACE("Scene loaded");
	}
	void Scene::Dispose()
	{
		// TODO: dispose something...
	}
}