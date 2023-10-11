#include "precompiled.h"
#include "Scene.h"
#include "Entity.h"
#include "Components.h"

namespace Eklipse
{
	static Mesh				s_viking;
	static Ref<Shader>		s_meshShader;
	static Ref<Shader>		s_spriteShader;
	static Ref<Material>	s_material;

	void Scene::Load()
	{
		EK_CORE_TRACE("Begin scene load");

		s_meshShader	= Shader::Create("Assets/Shaders/mesh.glsl");
		s_material		= Material::Create(s_meshShader);
		s_viking		= { "Assets/Models/viking_room.obj" };

		CreateEntity("Main Camera").AddComponent<CameraComponent>();

		CreateEntity("Viking").AddComponent<MeshComponent>(&s_viking, s_material.get());
		CreateEntity("Viking 2").AddComponent<MeshComponent>(&s_viking, s_material.get());

		EK_CORE_TRACE("Scene loaded");
	}
	void Scene::Dispose()
	{
		
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