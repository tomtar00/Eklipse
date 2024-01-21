#include "precompiled.h"
#include "Scene.h"
#include "Entity.h"
#include "Components.h"

#include <Eklipse/Core/Application.h>
#include <dylib.hpp>

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

	Scene::Scene() : m_name("Empty Scene"), m_path("NULL")
	{
	}
	Scene::Scene(const std::string& name, const std::filesystem::path& saveFilePath) :
		m_name(name), m_path(saveFilePath)
	{
		if (m_name.empty())
			m_name = "Untitled";
	}
	Scene::~Scene()
	{
		EK_CORE_TRACE("Begin scene '{0}' destruction", m_name);

		OnSceneStop();

		EK_CORE_TRACE("Scene '{0}' destroyed", m_name);
	}

	// Scene Events
	void Scene::OnSceneStart()
	{
		EK_CORE_TRACE("Starting scene '{0}'", m_name);

		// set main camera
		m_registry.view<CameraComponent>().each([&](auto entityID, auto& cameraComponent)
		{
			if (cameraComponent.camera.m_isMain)
			{
				m_mainCamera = &cameraComponent.camera;
				m_mainCameraTransform = &m_registry.get<TransformComponent>(entityID).transform;
			}
		});

		m_registry.view<ScriptComponent>().each([&](auto entityID, auto& scriptComponent)
		{
			EK_ASSERT(scriptComponent.script, "Script is null");
			scriptComponent.script->OnCreate();
		});

		m_state = SceneState::RUNNING;
	}
	void Scene::OnSceneUpdate(float deltaTime)
	{
		EK_PROFILE();

		try
		{
			m_registry.view<ScriptComponent>().each([&](auto entityID, auto& scriptComponent)
			{
				EK_ASSERT(scriptComponent.script, "Script is null");
				scriptComponent.script->OnUpdate(deltaTime);
			});
		}
		catch (const std::exception& e)
		{
			EK_CORE_ERROR("Exception thrown in script: {0}", e.what());
		}
	}
	void Scene::OnScenePause()
	{
		EK_CORE_TRACE("Pausing scene '{0}'", m_name);

		m_state = SceneState::PAUSED;
	}
	void Scene::OnSceneResume()
	{
		EK_CORE_TRACE("Resuming scene '{0}'", m_name);

		m_state = SceneState::RUNNING;
	}
	void Scene::OnSceneStop()
	{
		EK_CORE_TRACE("Stoping scene '{0}'", m_name);

		DestroyAllScripts();

		m_state = SceneState::NONE;
	}

	/*void Scene::ReloadScripts(Ref<Scene> scene)
	{
		EK_CORE_INFO("Reloading scripts on scene '{}'", scene->GetName());

		auto& scriptClasses = Eklipse::Project::GetActive()->GetScriptModule()->GetClasses();
		scene->GetRegistry().view<ScriptComponent>().each([&](auto entityID, auto& scriptComponent)
		{
			scriptComponent.DestroyScript();
			auto it = scriptClasses.find(scriptComponent.scriptName);
			if (it != scriptClasses.end())
			{
				scriptComponent.SetScript(it->first, it->second, Entity(entityID, scene.get()));
			}
			else
				EK_CORE_ERROR("Failed to reload script '{0}'", scriptComponent.scriptName);
		});

		Eklipse::SceneSerializer serializer(scene);
		serializer.DeserializeAllScriptProperties();
	}*/

	// Scene
	Ref<Scene> Scene::Copy(Ref<Scene> other)
	{
		EK_CORE_TRACE("Copying scene '{0}'", other->m_name);

		Ref<Scene> newScene = CreateRef<Scene>(other->m_name + " Copy", other->m_path.path());

		auto& srcSceneRegistry = other->m_registry;
		auto& dstSceneRegistry = newScene->m_registry;
		std::unordered_map<UUID, entt::entity> enttMap;

		srcSceneRegistry.view<IDComponent>().each([&](auto entityID, auto& idComponent)
		{
			UUID uuid = idComponent.ID;
			const auto& name = srcSceneRegistry.get<NameComponent>(entityID).name;
			Entity newEntity = newScene->CreateEntity(uuid, name);
			enttMap[uuid] = newEntity.GetHandle();

			EK_CORE_TRACE("Copying entity with ID = {0}, name = {1}", uuid, name);
		});

		CopyComponent(AllComponents{}, dstSceneRegistry, srcSceneRegistry, enttMap);

		auto view = srcSceneRegistry.view<ScriptComponent>();
		for (auto srcEntity : view)
		{
			UUID uuid = srcSceneRegistry.get<IDComponent>(srcEntity).ID;
			entt::entity dstEntity = enttMap.at(uuid);
			Entity dstEntityHandle = { dstEntity, newScene.get() };

			auto& srcComponent = srcSceneRegistry.get<ScriptComponent>(srcEntity);
			auto& dstComponent = dstEntityHandle.AddComponent<ScriptComponent>();
			dstComponent.SetScript(srcComponent.scriptName, srcComponent.classInfo, dstEntityHandle);

			EK_CORE_TRACE("Copying script '{0}' of entity with ID = {1}, name = {2}", srcComponent.scriptName, uuid, dstEntityHandle.GetName());

			// TODO: refactor
			for (auto& [memberName, member] : srcComponent.classInfo.members)
			{
				if (member.type == "int")
				{
					int value = *srcComponent.GetScriptValue<int>(member.offset);
					dstComponent.SetScriptValue(member.offset, value);
					EK_CORE_TRACE("Copying script property '{}' of entity with ID = {}, value = {}", memberName, uuid, value);
				}
				else if (member.type == "float")
				{
					float value = *srcComponent.GetScriptValue<float>(member.offset);
					dstComponent.SetScriptValue(member.offset, value);
					EK_CORE_TRACE("Copying script property '{}' of entity with ID = {}, value = {}", memberName, uuid, value);
				}
				else if (member.type == "bool")
				{
					bool value = *srcComponent.GetScriptValue<bool>(member.offset);
					dstComponent.SetScriptValue(member.offset, value);
					EK_CORE_TRACE("Copying script property '{}' of entity with ID = {}, value = {}", memberName, uuid, value);
				}
				else if (member.type == "std::string")
				{
					std::string& value = *srcComponent.GetScriptValue<std::string>(member.offset);
					dstComponent.SetScriptValue(member.offset, value);
					EK_CORE_TRACE("Copying script property '{}' of entity with ID = {}, value = {}", memberName, uuid, value);
				}
				else if (member.type == "glm::vec2")
					dstComponent.SetScriptValue(member.offset, *srcComponent.GetScriptValue<glm::vec2>(member.offset));
				else if (member.type == "glm::vec3")
					dstComponent.SetScriptValue(member.offset, *srcComponent.GetScriptValue<glm::vec3>(member.offset));
				else if (member.type == "glm::vec4")
					dstComponent.SetScriptValue(member.offset, *srcComponent.GetScriptValue<glm::vec4>(member.offset));
			}
		}
		return newScene;
	}
	Ref<Scene> Scene::New(const std::string& name, const std::filesystem::path& saveFilePath)
	{
		auto scene = CreateRef<Scene>(name, saveFilePath);
		Save(scene);

		EK_CORE_TRACE("Created new scene '{0}' at location '{1}'", scene->GetName(), saveFilePath.string());

		return scene;
	}
	Ref<Scene> Scene::Load(const std::filesystem::path& saveFilePath, const Ref<dylib>& library)
	{
		EK_CORE_TRACE("Loading scene from '{0}'", saveFilePath.string());

		Ref<Scene> scene = CreateRef<Scene>("ToBeLoaded", saveFilePath);
		if (!scene->Deserialize(library))
		{
			EK_CORE_ERROR("Failed to load scene '{0}'", saveFilePath.string());
			return nullptr;
		}

		EK_CORE_TRACE("Scene '{0}' loaded", scene->GetName());
		return scene;
	}
	void Scene::Save(Ref<Scene> scene)
	{
		EK_CORE_TRACE("Saving scene '{0}'", scene->GetName());

		if (!scene->Serialize())
		{
			EK_CORE_ERROR("Failed to save scene '{0}'", scene->GetName());
		}

		EK_CORE_TRACE("Scene '{0}' saved to path '{1}'", scene->GetName(), scene->GetPath().string());
	}

	// Scripts
	void Scene::DestroyAllScripts()
	{
		EK_CORE_TRACE("Destroying all scripts on scene '{0}'", m_name);

		m_registry.view<ScriptComponent>().each([&](auto entityID, auto& scriptComponent)
		{
			scriptComponent.DestroyScript();
		});
	}
	void Scene::InitializeAllScripts()
	{
		EK_CORE_TRACE("Initializing all scripts on scene '{0}'", m_name);

		auto& scriptClasses = Eklipse::Project::GetActive()->GetScriptModule()->GetClasses();
		m_registry.view<ScriptComponent>().each([&](auto entityID, auto& scriptComponent)
		{
			if (scriptComponent.script != nullptr)
			{
				EK_CORE_ERROR("Script '{0}' has never been destroyed!", scriptComponent.scriptName);
			}
			else
			{
				auto it = scriptClasses.find(scriptComponent.scriptName);
				if (it == scriptClasses.end())
				{
					EK_CORE_ERROR("Failed to initialize script '{0}'", scriptComponent.scriptName);
				}
				scriptComponent.SetScript(it->first, it->second, Entity(entityID, this));
			}
		});

		DeserializeEveryScriptProperties();
	}

	// Components
	void Scene::ApplyAllComponents()
	{
		EK_CORE_TRACE("Applying all components on scene '{0}'", m_name);

		m_registry.view<MeshComponent>().each([&](auto entityID, auto& meshComponent)
		{
			if (!meshComponent.meshPath.empty() && !meshComponent.materialPath.empty())
			{
				meshComponent.mesh = Application::Get().GetMainAssetLibrary()->GetMesh(meshComponent.meshPath).get();
				meshComponent.material = Application::Get().GetMainAssetLibrary()->GetMaterial(meshComponent.materialPath).get();
			}
		});
	}

	// Entity
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

		EK_CORE_ERROR("Failed to find entity {0} on scene '{1}'", uuid, m_name);
		return {};
	}
	void Scene::DestroyEntity(Entity entity)
	{
		m_entityMap.erase(entity.GetUUID());
		m_registry.destroy(entity.GetHandle());
	}

	// Serialization
	bool Scene::Serialize()
	{
		EK_CORE_TRACE("Serializing scene '{0}'...", m_name);

		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Untitled";
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		m_registry.each([&](auto entityID)
		{
			Entity entity = { entityID, this };
			SerializeEntity(entity, out);
		});
		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(m_path.full_path());
		fout << out.c_str();

		EK_CORE_TRACE("Serialized scene '{0}' to path '{1}'", m_name, m_path.string());
		return true;
	}
	bool Scene::SerializeEntity(Entity entity, YAML::Emitter& out)
	{
		EK_ASSERT(entity.HasComponent<IDComponent>(), "Tried to serialize invalid entity!");

		out << YAML::BeginMap;
		out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();

		EK_CORE_TRACE("Serializing entity with ID = {0}", entity.GetUUID());

		if (entity.HasComponent<NameComponent>())
		{
			out << YAML::Key << "NameComponent";
			out << YAML::BeginMap;

			auto& tag = entity.GetComponent<NameComponent>().name;
			out << YAML::Key << "Name" << YAML::Value << tag;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<TransformComponent>())
		{
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap;

			auto& tc = entity.GetComponent<TransformComponent>();
			out << YAML::Key << "Position" << YAML::Value << tc.transform.position;
			out << YAML::Key << "Rotation" << YAML::Value << tc.transform.rotation;
			out << YAML::Key << "Scale" << YAML::Value << tc.transform.scale;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<CameraComponent>())
		{
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap;

			auto& cameraComponent = entity.GetComponent<CameraComponent>();
			auto& camera = cameraComponent.camera;

			out << YAML::Key << "Camera" << YAML::Value;
			out << YAML::BeginMap;
			out << YAML::Key << "IsMain" << YAML::Value << camera.m_isMain;
			out << YAML::Key << "FOV" << YAML::Value << camera.m_fov;
			out << YAML::Key << "Near" << YAML::Value << camera.m_nearPlane;
			out << YAML::Key << "Far" << YAML::Value << camera.m_farPlane;
			out << YAML::EndMap;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<MeshComponent>())
		{
			out << YAML::Key << "MeshComponent";
			out << YAML::BeginMap;

			auto& meshComponent = entity.GetComponent<MeshComponent>();
			out << YAML::Key << "Mesh" << YAML::Value << meshComponent.meshPath.string();
			out << YAML::Key << "Material" << YAML::Value << meshComponent.materialPath.string();

			out << YAML::EndMap;
		}

		if (entity.HasComponent<ScriptComponent>())
		{
			EK_ASSERT(Project::GetActive(), "No active project!");

			out << YAML::Key << "ScriptComponent";
			out << YAML::BeginMap;

			auto& scriptComponent = entity.GetComponent<ScriptComponent>();
			out << YAML::Key << "Name" << YAML::Value << scriptComponent.scriptName;

			out << YAML::Key << "Properties" << YAML::Value;
			out << YAML::BeginMap;

			std::vector<std::string> toRemove;
			for (auto&& [name, member] : scriptComponent.classInfo.members)
			{
				auto& parsedMembers = Project::GetActive()->GetScriptModule()->GetClasses()[scriptComponent.scriptName].members;
				auto it = parsedMembers.find(name);
				if (it == parsedMembers.end()) // Serialize only when the script has the property
				{
					toRemove.push_back(name);
					continue;
				}

				if (scriptComponent.script != nullptr)
				{
					if (member.type == "int")
						out << YAML::Key << name << YAML::Value << *scriptComponent.GetScriptValue<int>(member.offset);
					else if (member.type == "float")
						out << YAML::Key << name << YAML::Value << *scriptComponent.GetScriptValue<float>(member.offset);
					else if (member.type == "bool")
						out << YAML::Key << name << YAML::Value << *scriptComponent.GetScriptValue<bool>(member.offset);
					else if (member.type == "std::string")
						out << YAML::Key << name << YAML::Value << *scriptComponent.GetScriptValue<std::string>(member.offset);
					else if (member.type == "glm::vec2")
						out << YAML::Key << name << YAML::Value << *scriptComponent.GetScriptValue<glm::vec2>(member.offset);
					else if (member.type == "glm::vec3")
						out << YAML::Key << name << YAML::Value << *scriptComponent.GetScriptValue<glm::vec3>(member.offset);
					else if (member.type == "glm::vec4")
						out << YAML::Key << name << YAML::Value << *scriptComponent.GetScriptValue<glm::vec4>(member.offset);
					else if (member.type == "glm::mat4")
						out << YAML::Key << name << YAML::Value << *scriptComponent.GetScriptValue<glm::mat4>(member.offset);
					else
						EK_CORE_WARN("Unknown type '{0}' for script property '{1}' while serializing entity {2}", member.type, name, entity.GetUUID());
				}
				else
				{
					EK_CORE_WARN("Script '{0}' has never been initialized!", scriptComponent.scriptName);
				}
			}
			for (auto& name : toRemove)
				scriptComponent.classInfo.members.erase(name);
			out << YAML::EndMap;
			out << YAML::EndMap;
		}
		out << YAML::EndMap;

		EK_CORE_TRACE("Serialized entity with ID = {0}", entity.GetUUID());

		return true;
	}
	bool Scene::Deserialize(const Ref<dylib>& library)
	{
		EK_CORE_TRACE("Deserializing scene '{0}'...", m_name);

		YAML::Node data;
		try
		{
			data = YAML::LoadFile(m_path.full_string());
		}
		catch (const std::exception& e)
		{
			EK_CORE_ERROR("Failed to load .eksc file '{0}'\n\t{1}", m_path.full_string(), e.what());
			return false;
		}

		if (!data["Scene"])
			return false;

		std::string sceneName;
		TryDeserailize<std::string>(data, "Scene", &sceneName);
		SetName(sceneName);

		auto entities = data["Entities"];
		if (entities)
		{
			for (auto entityNode : entities)
			{
				DeserializeEntity(entityNode, library);
			}
		}

		EK_CORE_TRACE("Deserialized scene '{0}'", sceneName);

		return true;
	}
	bool Scene::DeserializeEntity(YAML::Node& entityNode, const Ref<dylib>& library)
	{
		uint64_t uuid;
		TryDeserailize<uint64_t>(entityNode, "Entity", &uuid);

		std::string name;
		auto nameComponent = entityNode["NameComponent"];
		if (nameComponent)
		{
			TryDeserailize<std::string>(nameComponent, "Name", &name);
		}

		EK_CORE_TRACE("Deserialing entity with ID = {0}, name = {1}", uuid, name);

		Entity deserializedEntity = CreateEntity(uuid, name);

		auto transformComponent = entityNode["TransformComponent"];
		if (transformComponent)
		{
			auto& tc = deserializedEntity.GetComponent<TransformComponent>();

			TryDeserailize<glm::vec3>(transformComponent, "Position", &tc.transform.position);
			TryDeserailize<glm::vec3>(transformComponent, "Rotation", &tc.transform.rotation);
			TryDeserailize<glm::vec3>(transformComponent, "Scale", &tc.transform.scale);
		}

		auto cameraComponent = entityNode["CameraComponent"];
		if (cameraComponent)
		{
			auto& cc = deserializedEntity.AddComponent<CameraComponent>();
			auto& cameraProps = cameraComponent["Camera"];

			TryDeserailize<bool>(cameraProps, "IsMain", &cc.camera.m_isMain);
			TryDeserailize<float>(cameraProps, "FOV", &cc.camera.m_fov);
			TryDeserailize<float>(cameraProps, "Near", &cc.camera.m_nearPlane);
			TryDeserailize<float>(cameraProps, "Far", &cc.camera.m_farPlane);
		}

		auto meshComponent = entityNode["MeshComponent"];
		if (meshComponent)
		{
			auto& mc = deserializedEntity.AddComponent<MeshComponent>();

			mc.meshPath = TryDeserailize<std::string>(meshComponent, "Mesh", "");
			mc.materialPath = TryDeserailize<std::string>(meshComponent, "Material", "");
		}

		auto scriptComponent = entityNode["ScriptComponent"];
		if (scriptComponent)
		{
			auto scriptName = TryDeserailize<std::string>(scriptComponent, "Name", "");
			if (library)
			{
				auto& sc = deserializedEntity.AddComponent<ScriptComponent>();
				sc.scriptName = scriptName;

				if (!scriptName.empty())
				{
					EklipseEngine::Reflections::ClassInfo info{};
					library->get_function<void(EklipseEngine::Reflections::ClassInfo&)>("Get__" + sc.scriptName)(info);
					sc.SetScript(sc.scriptName, info, deserializedEntity);

					DeserializeScriptProperties(deserializedEntity, scriptComponent["Properties"]);
				}
				else
				{
					EK_CORE_WARN("Entity {0} has a script component but no script name was provided to instanitiate it!", deserializedEntity.GetUUID());
				}
			}
			else
			{
				EK_CORE_WARN("Entity {0} has a script component '{1}' but no library was provided to instanitiate it!", deserializedEntity.GetUUID(), scriptName);
			}
		}

		EK_CORE_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid, name);

		return true;
	}
	bool Scene::DeserializeEveryScriptProperties()
	{
		EK_CORE_TRACE("Deserializing all script properties of scene '{0}'...", m_name);

		YAML::Node data;
		try
		{
			data = YAML::LoadFile(m_path.full_string());
		}
		catch (const std::exception& e)
		{
			EK_CORE_ERROR("Failed to load .eksc file '{0}'\n     {1}", m_path.full_string(), e.what());
			return false;
		}

		if (!data["Scene"])
			return false;

		std::string sceneName = data["Scene"].as<std::string>();

		auto entitiesNodes = data["Entities"];
		for (auto entityNode : entitiesNodes)
		{
			Entity entity = GetEntity(entityNode["Entity"].as<uint64_t>());

			auto scriptComponent = entityNode["ScriptComponent"];
			if (scriptComponent)
			{
				auto properties = scriptComponent["Properties"];
				if (properties)
					DeserializeScriptProperties(entity, properties);
			}
		}

		EK_CORE_TRACE("Deserialized all script properties of scene '{0}'", sceneName);

		return true;
	}
	bool Scene::DeserializeScriptProperties(Entity entity, YAML::Node& propertiesNode)
	{
		EK_CORE_TRACE("Deserializing script properties of entity {0}", entity.GetUUID());

		if (propertiesNode)
		{
			auto& sc = entity.GetComponent<ScriptComponent>();
			for (auto propertyNode : propertiesNode)
			{
				std::string name = propertyNode.first.as<std::string>();
				std::string type = sc.classInfo.members[name].type;
				uint32_t offset = sc.classInfo.members[name].offset;

				if (type == "int")
					sc.SetScriptValue<int>(offset, TryDeserailize(propertiesNode, name, 0));
				else if (type == "float")
					sc.SetScriptValue<float>(offset, TryDeserailize(propertiesNode, name, 0.0f));
				else if (type == "bool")
					sc.SetScriptValue<bool>(offset, TryDeserailize(propertiesNode, name, false));
				else if (type == "std::string")
					sc.SetScriptValue<std::string>(offset, TryDeserailize(propertiesNode, name, std::string{}));
				else if (type == "glm::vec2")
					sc.SetScriptValue<glm::vec2>(offset, TryDeserailize(propertiesNode, name, glm::vec2{}));
				else if (type == "glm::vec3")
					sc.SetScriptValue<glm::vec3>(offset, TryDeserailize(propertiesNode, name, glm::vec3{}));
				else if (type == "glm::vec4")
					sc.SetScriptValue<glm::vec4>(offset, TryDeserailize(propertiesNode, name, glm::vec4{}));
				else if (type == "glm::mat4")
					sc.SetScriptValue<glm::mat4>(offset, TryDeserailize(propertiesNode, name, glm::mat4 {}));
				else
					EK_CORE_WARN("Unknown type '{0}' for script property '{1}' while deserializing script properties on entity {2}", type, name, entity.GetUUID());

				EK_CORE_TRACE("Deserialized script property '{0}' of entity {1}", name, entity.GetUUID());
			}	
		}

		EK_CORE_TRACE("Deserialized script properties of entity {0}", entity.GetUUID());

		return true;
	}
}