#include "precompiled.h"
#include "Scene.h"
#include "Entity.h"
#include "Components.h"
#include "SceneSerializer.h"
#include <Eklipse/Core/Application.h>

namespace Eklipse
{
	template<typename... Component>
	static void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
	{
		([&]()
		{
			auto view = src.view<Component>();
			for (auto srcEntity : view)
			{
				entt::entity dstEntity = enttMap.at(src.get<IDComponent>(srcEntity).ID);

				auto& srcComponent = src.get<Component>(srcEntity);
				dst.emplace_or_replace<Component>(dstEntity, srcComponent);
			}
		}(), ...);
	}
	/*static void CopyScriptComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
	{
		auto view = src.view<ScriptComponent>();
		for (auto srcEntity : view)
		{
			entt::entity dstEntity = enttMap.at(src.get<IDComponent>(srcEntity).ID);

			auto& scriptComponent = src.get<ScriptComponent>(srcEntity);
			scriptComponent.SetScript(scriptComponent.scriptName, scriptComponent.classInfo, Entity(dstEntity, this));
			dst.emplace_or_replace<ScriptComponent>(dstEntity, scriptComponent);
		}
	}*/

	template<typename... Component>
	static void CopyComponent(ComponentGroup<Component...>, entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
	{
		CopyComponent<Component...>(dst, src, enttMap);
	}

	template<typename... Component>
	static void CopyComponentIfExists(Entity dst, Entity src)
	{
		([&]()
		{
			if (src.HasComponent<Component>())
				dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
		}(), ...);
	}

	template<typename... Component>
	static void CopyComponentIfExists(ComponentGroup<Component...>, Entity dst, Entity src)
	{
		CopyComponentIfExists<Component...>(dst, src);
	}

	Scene::Scene(const std::string& name, const Path& saveFilePath) : 
		m_name(name), m_path(saveFilePath)
	{
		if (m_name.empty())
			m_name = "Untitled";
	}
	Scene::~Scene()
	{
		EK_CORE_TRACE("Begin scene '{0}' unload", m_name);

		OnSceneStop();

		EK_CORE_TRACE("Scene disposed");
	}

	void Scene::ApplyAllComponents()
	{
		m_registry.view<MeshComponent>().each([&](auto entityID, auto& meshComponent)
		{
			if (!meshComponent.meshPath.empty() && !meshComponent.materialPath.empty())
			{
				meshComponent.mesh = Eklipse::Project::GetActive()->GetAssetLibrary()->GetMesh(meshComponent.meshPath).get();
				meshComponent.material = Eklipse::Project::GetActive()->GetAssetLibrary()->GetMaterial(meshComponent.materialPath).get();
			}
		});
	}

	void Scene::OnSceneStart()
	{
		// set main camera
		m_registry.view<CameraComponent>().each([&](auto entityID, auto& cameraComponent)
		{
			if (*cameraComponent.camera.IsMain())
			{
				m_mainCamera = &cameraComponent.camera;
				m_mainCameraTransform = &m_registry.get<TransformComponent>(entityID).transform;
			}
		});

		m_registry.view<ScriptComponent>().each([&](auto entityID, auto& scriptComponent)
		{
			if (scriptComponent.script != nullptr)
				scriptComponent.script->OnCreate();
		});
	}
	void Scene::OnSceneUpdate(float deltaTime)
	{
		EK_PROFILE();

		m_registry.view<ScriptComponent>().each([&](auto entityID, auto& scriptComponent)
		{
			if (scriptComponent.script != nullptr)
				scriptComponent.script->OnUpdate(deltaTime);
		});
	}
	void Scene::OnSceneStop()
	{
		m_registry.view<ScriptComponent>().each([&](auto entityID, auto& scriptComponent)
		{
			scriptComponent.DestroyScript();
		});
	}

	void Scene::ReloadScripts(Ref<Scene> scene)
	{
		EK_CORE_INFO("Reloading scripts...");

		auto& scriptClasses = Eklipse::Project::GetScriptClasses();
		scene->GetRegistry().view<ScriptComponent>().each([&](auto entityID, auto& scriptComponent)
		{
			scriptComponent.DestroyScript();
			auto it = scriptClasses.find(scriptComponent.scriptName);
			if (it != scriptClasses.end())
				scriptComponent.SetScript(it->first, it->second, Entity(entityID, scene.get()));
			else
				EK_CORE_ERROR("Failed to reload script '{0}'", scriptComponent.scriptName);
		});

		Eklipse::SceneSerializer serializer(scene);
		serializer.DeserializeAllScriptProperties();
	}

	Ref<Scene> Scene::Copy(Ref<Scene> other)
	{
		Ref<Scene> newScene = CreateRef<Scene>();

		auto& srcSceneRegistry = other->m_registry;
		auto& dstSceneRegistry = newScene->m_registry;
		std::unordered_map<UUID, entt::entity> enttMap;

		srcSceneRegistry.view<IDComponent>().each([&](auto entityID, auto& idComponent)
		{
			UUID uuid = idComponent.ID;
			const auto& name = srcSceneRegistry.get<NameComponent>(entityID).name;
			Entity newEntity = newScene->CreateEntity(uuid, name);
			enttMap[uuid] = newEntity.GetHandle();
		});

		CopyComponent(AllComponents{}, dstSceneRegistry, srcSceneRegistry, enttMap);

		auto view = srcSceneRegistry.view<ScriptComponent>();
		for (auto srcEntity : view)
		{
			entt::entity dstEntity = enttMap.at(srcSceneRegistry.get<IDComponent>(srcEntity).ID);

			auto& srcComponent = srcSceneRegistry.get<ScriptComponent>(srcEntity);
			auto dstComponent = srcComponent;
			dstComponent.SetScript(dstComponent.scriptName, dstComponent.classInfo, Entity(dstEntity, newScene.get()));

			// TODO: refactor
			for (auto& [memberName, member] : srcComponent.classInfo.members)
			{
				if (member.type == "int")
					dstComponent.SetScriptValue(member.offset, *srcComponent.GetScriptValue<int>(member.offset));
				else if (member.type == "float")
					dstComponent.SetScriptValue(member.offset, *srcComponent.GetScriptValue<float>(member.offset));
				else if (member.type == "bool")
					dstComponent.SetScriptValue(member.offset, *srcComponent.GetScriptValue<bool>(member.offset));
				else if (member.type == "std::string")
					dstComponent.SetScriptValue(member.offset, *srcComponent.GetScriptValue<std::string>(member.offset));
				else if (member.type == "glm::vec2")
					dstComponent.SetScriptValue(member.offset, *srcComponent.GetScriptValue<glm::vec2>(member.offset));
				else if (member.type == "glm::vec3")
					dstComponent.SetScriptValue(member.offset, *srcComponent.GetScriptValue<glm::vec3>(member.offset));
				else if (member.type == "glm::vec4")
					dstComponent.SetScriptValue(member.offset, *srcComponent.GetScriptValue<glm::vec4>(member.offset));
			}

			dstSceneRegistry.emplace_or_replace<ScriptComponent>(dstEntity, dstComponent);
		}

		return newScene;
	}

	Ref<Scene> Scene::New(const std::string& name, const Path& saveFilePath)
	{
		auto scene = CreateRef<Scene>(name, saveFilePath);
		Save(scene);

		EK_CORE_TRACE("Created new scene '{0}' at location '{1}'", scene->GetName(), saveFilePath.full_string());

		return scene;
	}
	void Scene::Save(Ref<Scene> scene)
	{
		EK_CORE_TRACE("Saving scene '{0}'", scene->GetName());

		Eklipse::SceneSerializer serializer(scene);
		serializer.Serialize(scene->GetPath());

		EK_CORE_TRACE("Scene '{0}' saved to path '{1}'", scene->GetName(), scene->GetPath().full_string());
	}
	Ref<Scene> Scene::Load(const Path& saveFilePath)
	{
		EK_CORE_TRACE("Loading scene from '{0}'", saveFilePath.full_string());

		Ref<Scene> scene = CreateRef<Scene>();
		Eklipse::SceneSerializer serializer(scene);
		if (serializer.Deserialize(saveFilePath))
		{
			EK_CORE_TRACE("Scene '{0}' loaded", scene->GetName());
			return scene;
		}

		EK_CORE_ERROR("Failed to load scene '{0}'", saveFilePath.full_string());
		return nullptr;
	}

	Entity Scene::CreateEntity(const std::string name)
	{
		return CreateEntity(UUID(), name);
	}
	Entity Scene::CreateEntity(UUID uuid, const std::string& name)
	{
		Entity entity = { m_registry.create(), this };
		entity.AddComponent<IDComponent>(uuid);
		entity.AddComponent<NameComponent>(name.empty() ? "Empty Entity" : name);
		entity.AddComponent<TransformComponent>();

		m_entityMap[uuid] = entity.GetHandle();

		return entity;
	}
	Entity Scene::GetEntity(UUID uuid)
	{
		auto it = m_entityMap.find(uuid);
		if (it != m_entityMap.end())
			return { it->second, this };
		return {};
	}
	void Scene::DestroyEntity(Entity entity)
	{
		m_entityMap.erase(entity.GetUUID());
		m_registry.destroy(entity.GetHandle());
	}
}