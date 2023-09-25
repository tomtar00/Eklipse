#include "precompiled.h"
#include "Scene.h"
#include "Components.h"

namespace Eklipse
{
	void Scene::Load()
	{
		EK_CORE_TRACE("Begin scene load");

		CreateEntity("Main Camera").AddComponent<CameraComponent>();

		//m_camera.m_name = "Main Camera";
		//m_camera.m_fov = 65.0f;
		//m_camera.m_transform.position = { 0.0f,  0.0f,  1.0f };
		//m_camera.m_transform.rotation = { 0.0f,  0.0f,  0.0f };
		//m_camera.m_transform.scale =	{ 1.0f,  1.0f,  1.0f };

		//std::string entityName{};

		//// xyzrgb_dragon.obj
		//Mesh dragonMesh{ "models/xyzrgb_dragon.obj", "textures/viking_room.png" };
		//entityName = "Dragon";
		//Entity dragon{ entityName, dragonMesh };

		//dragon.m_transform.position =	{ 1.5f,	0.0f, -15.0f };
		//dragon.m_transform.rotation =	{ 0.0f,	0.0f,   0.0f };
		//dragon.m_transform.scale =		{ 0.1f,	0.1f,   0.1f };

		//m_entities.push_back(dragon);

		//// viking_room.obj
		//Mesh vikingMesh{ "models/viking_room.obj", "textures/viking_room.png" };
		//entityName = "Viking";
		//Entity viking{ entityName, vikingMesh };

		//viking.m_transform.position =	{   0.0f,  -0.5f,   -1.0f };
		//viking.m_transform.rotation =	{ -90.0f,   0.0f,  -90.0f };
		//viking.m_transform.scale =		{   1.0f,   1.0f,    1.0f };

		//m_entities.push_back(viking);

		EK_CORE_TRACE("Scene loaded");
	}
	void Scene::Dispose()
	{
		// TODO: dispose something...
	}
	Entity Scene::CreateEntity(const std::string name)
	{
		Entity entity = { m_registry.create(), this };
		entity.AddComponent<NameComponent>(name.empty() ? "Empty Entity" : name);
		entity.AddComponent<TransformComponent>();
		return entity;
	}
}