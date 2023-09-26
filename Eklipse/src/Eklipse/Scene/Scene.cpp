#include "precompiled.h"
#include "Scene.h"
#include "Entity.h"
#include "Components.h"

namespace Eklipse
{
	Mesh viking;

	void Scene::Load()
	{
		EK_CORE_TRACE("Begin scene load");

		CreateEntity("Main Camera").AddComponent<CameraComponent>();

		viking = { "models/viking_room.obj", "textures/viking_room.png" };
		CreateEntity("Viking").AddComponent<MeshComponent>(viking);
		CreateEntity("Viking 2").AddComponent<MeshComponent>(viking);

		EK_CORE_TRACE("Scene loaded");
	}
	void Scene::Dispose()
	{
		// TODO: dispose something...
	}
	void Scene::CreateTestMesh(const std::string& name)
	{
		CreateEntity(name).AddComponent<MeshComponent>(viking);
	}
	Entity Scene::CreateEntity(const std::string name)
	{
		Entity entity = { m_registry.create(), this };
		entity.AddComponent<NameComponent>(name.empty() ? "Empty Entity" : name);
		entity.AddComponent<TransformComponent>();
		return entity;
	}
	void Scene::DestroyEntity(Entity entity)
	{
		m_registry.destroy(entity.m_entityHandle);
	}
}