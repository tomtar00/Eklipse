#include "precompiled.h"
#include "Scene.h"
#include "Entity.h"
#include "Components.h"
#include "Assets.h"

#include <yaml-cpp/yaml.h>

namespace Eklipse
{
	//static Ref<Shader>			s_defaultShader;
	//static Ref<Shader>		s_spriteShader;
	//static Ref<Material>		s_material;

	Scene::Scene()
	{
		CreateEntity("Main Camera").AddComponent<CameraComponent>();
	}

	void Scene::Load()
	{
		EK_CORE_TRACE("Begin scene load");

		//s_spriteShader	= Assets::GetShader("Assets/Shaders/sprite.glsl");
		//s_defaultShader	= Assets::GetShader("Assets/Shaders/DefaultShader.eksh");
		//s_material		= Material::Create(s_meshShader);
		//
		//Ref<Mesh> viking = Assets::GetMesh("Assets/Models/viking_room.obj");

		
		//CreateEntity("Viking").AddComponent<MeshComponent>(viking.get(), s_material.get());
		//CreateEntity("Viking 2").AddComponent<MeshComponent>(viking.get(), s_material.get());

		EK_CORE_TRACE("Scene loaded");
	}
	void Scene::Dispose()
	{
		EK_CORE_TRACE("Begin scene dispose");

		//s_spriteShader	= nullptr;
		//s_defaultShader	= nullptr;
		//s_material		= nullptr;

		m_registry.clear();

		EK_CORE_TRACE("Scene disposed");
	}
	Entity Scene::CreateEntity(const std::string name)
	{
		Entity entity = { m_registry.create(), this };
		entity.AddComponent<IDComponent>();
		entity.AddComponent<NameComponent>(name.empty() ? "Empty Entity" : name);
		entity.AddComponent<TransformComponent>();
		return entity;
	}
	void Scene::DestroyEntity(Entity entity)
	{
		m_registry.destroy(entity.GetHandle());
	}
}