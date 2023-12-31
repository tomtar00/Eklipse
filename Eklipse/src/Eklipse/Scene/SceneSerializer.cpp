#include "precompiled.h"
#include "SceneSerializer.h"
#include "Components.h"

#include <Eklipse/Utils/Yaml.h>

namespace Eklipse
{
	SceneSerializer::SceneSerializer(const Ref<Scene> scene) : m_scene(scene)
	{
	}

	bool SceneSerializer::Serialize(const Path& targetFilePath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Untitled";
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		m_scene->GetRegistry().each([&](auto entityID)
		{
			Entity entity = { entityID, m_scene.get() };
			SerializeEntity(out, entity);
		});
		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(targetFilePath.full_path());
		fout << out.c_str();

		EK_CORE_TRACE("Serialized scene '{0}' to path '{1}'", m_scene->GetName(), targetFilePath.full_string());
		return true;
	}

	bool SceneSerializer::Deserialize(const Path& sourceFilePath)
	{
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(sourceFilePath);
		}
		catch (YAML::ParserException e)
		{
			EK_CORE_ERROR("Failed to load .eksc file '{0}'\n     {1}", sourceFilePath, e.what());
			return false;
		}

		if (!data["Scene"])
			return false;

		std::string sceneName;
		TryDeserailize<std::string>(data, "Scene", &sceneName);
		m_scene->SetName(sceneName);
		m_scene->SetPath(sourceFilePath);
		EK_CORE_DBG("Deserializing scene '{0}'", sceneName);

		auto entities = data["Entities"];
		if (entities)
		{
			for (auto entity : entities)
			{
				uint64_t uuid;
				TryDeserailize<uint64_t>(entity, "Entity", &uuid);

				std::string name;
				auto nameComponent = entity["NameComponent"];
				if (nameComponent)
				{
					TryDeserailize<std::string>(nameComponent, "Name", &name);
				}

				EK_CORE_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid, name);

				Entity deserializedEntity = m_scene->CreateEntity(uuid, name);

				auto transformComponent = entity["TransformComponent"];
				if (transformComponent)
				{
					auto& tc = deserializedEntity.GetComponent<TransformComponent>();

					TryDeserailize<glm::vec3>(transformComponent, "Position", &tc.transform.position);
					TryDeserailize<glm::vec3>(transformComponent, "Rotation", &tc.transform.rotation);
					TryDeserailize<glm::vec3>(transformComponent, "Scale", &tc.transform.scale);
				}

				auto cameraComponent = entity["CameraComponent"];
				if (cameraComponent)
				{
					auto& cc = deserializedEntity.AddComponent<CameraComponent>();
					auto& cameraProps = cameraComponent["Camera"];

					TryDeserailize<bool>(cameraProps, "IsMain", &cc.camera.m_isMain);
					TryDeserailize<float>(cameraProps, "FOV", &cc.camera.m_fov);
					TryDeserailize<float>(cameraProps, "Near", &cc.camera.m_nearPlane);
					TryDeserailize<float>(cameraProps, "Far", &cc.camera.m_farPlane);
				}

				auto meshComponent = entity["MeshComponent"];
				if (meshComponent)
				{
					auto& mc = deserializedEntity.AddComponent<MeshComponent>();

					TryDeserailize<std::string>(meshComponent, "Mesh", &mc.meshPath);
					TryDeserailize<std::string>(meshComponent, "Material", &mc.materialPath);
				}

				auto scriptComponent = entity["ScriptComponent"];
				if (scriptComponent)
				{
					auto& sc = deserializedEntity.AddComponent<ScriptComponent>();
					TryDeserailize<std::string>(scriptComponent, "Name", &sc.scriptName);

					sc.SetScript(sc.scriptName, Project::GetScriptClasses()[sc.scriptName], deserializedEntity);

					auto properties = scriptComponent["Properties"];
					DeserializeScriptProperties(deserializedEntity, properties);
				}
			}
		}

		return true;
	}

	void SceneSerializer::SerializeEntity(YAML::Emitter& out, Entity entity)
	{
		EK_ASSERT(entity.HasComponent<IDComponent>(), "Tried to serialize invalid entity!");

		out << YAML::BeginMap;
		out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();

		EK_CORE_DBG("Serializing entity with ID = {0}", entity.GetUUID());

		if (entity.HasComponent<IDComponent>())
		{
			out << YAML::Key << "IDComponent";
			out << YAML::BeginMap;

			auto& tag = entity.GetComponent<IDComponent>().ID;
			out << YAML::Key << "ID" << YAML::Value << tag;

			out << YAML::EndMap;
		}

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
			out << YAML::Key << "Mesh" << YAML::Value << meshComponent.meshPath;
			out << YAML::Key << "Material" << YAML::Value << meshComponent.materialPath;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<ScriptComponent>())
		{
			out << YAML::Key << "ScriptComponent";
			out << YAML::BeginMap;

			auto& scriptComponent = entity.GetComponent<ScriptComponent>();
			out << YAML::Key << "Name" << YAML::Value << scriptComponent.scriptName;

			out << YAML::Key << "Properties" << YAML::Value;
			out << YAML::BeginSeq;
			for (auto&& [name, member] : scriptComponent.classInfo.members)
			{
				out << YAML::BeginMap;
				out << YAML::Key << "Name" << YAML::Value << name;
				out << YAML::Key << "Type" << YAML::Value << member.type;
				out << YAML::Key << "Offset" << YAML::Value << member.offset;

				if (member.type == "int")
					out << YAML::Key << "Value" << YAML::Value << *scriptComponent.GetScriptValue<int>(member.offset);
				else if (member.type == "float")
					out << YAML::Key << "Value" << YAML::Value << *scriptComponent.GetScriptValue<float>(member.offset);
				else if (member.type == "bool")
					out << YAML::Key << "Value" << YAML::Value << *scriptComponent.GetScriptValue<bool>(member.offset);
				else if (member.type == "std::string")
					out << YAML::Key << "Value" << YAML::Value << *scriptComponent.GetScriptValue<std::string>(member.offset);
				else if (member.type == "glm::vec2")
					out << YAML::Key << "Value" << YAML::Value << *scriptComponent.GetScriptValue<glm::vec2>(member.offset);
				else if (member.type == "glm::vec3")
					out << YAML::Key << "Value" << YAML::Value << *scriptComponent.GetScriptValue<glm::vec3>(member.offset);
				else if (member.type == "glm::vec4")
					out << YAML::Key << "Value" << YAML::Value << *scriptComponent.GetScriptValue<glm::vec4>(member.offset);
				else if (member.type == "glm::mat4")
					out << YAML::Key << "Value" << YAML::Value << *scriptComponent.GetScriptValue<glm::mat4>(member.offset);
				else
					EK_CORE_WARN("Unknown type '{0}' for script property '{1}'", member.type, name);

				out << YAML::EndMap;
			}
			out << YAML::EndSeq;
			out << YAML::EndMap;
		}
		out << YAML::EndMap;
	}
	void SceneSerializer::DeserializeAllScriptProperties()
	{
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(m_scene->GetPath().full_string());
		}
		catch (YAML::ParserException e)
		{
			EK_CORE_ERROR("Failed to load .eksc file '{0}'\n     {1}", m_scene->GetPath().full_string(), e.what());
			return;
		}

		if (!data["Scene"])
			return;

		std::string sceneName = data["Scene"].as<std::string>();
		EK_CORE_DBG("Deserializing scene '{0}'", sceneName);

		auto entitiesNodes = data["Entities"];
		for (auto entityNode : entitiesNodes)
		{
			Entity entity = m_scene->GetEntity(entityNode["Entity"].as<uint64_t>());

			// get script component node
			auto scriptComponent = entityNode["ScriptComponent"];
			if (scriptComponent)
			{
				auto properties = scriptComponent["Properties"];
				if (properties)
					DeserializeScriptProperties(entity, properties);
			}
		}
	}
	void SceneSerializer::DeserializeScriptProperties(Entity entity, YAML::Node& propertiesNode)
	{
		if (propertiesNode)
		{
			auto& sc = entity.GetComponent<ScriptComponent>();
			for (auto property : propertiesNode)
			{
				std::string name = property["Name"].as<std::string>();
				std::string type = property["Type"].as<std::string>();
				uint32_t offset = property["Offset"].as<uint32_t>();

				if (type == "int")
					sc.SetScriptValue<int>(offset, TryDeserailize(property, "Value", 0));
				else if (type == "float")
					sc.SetScriptValue<float>(offset, TryDeserailize(property, "Value", 0.0f));
				else if (type == "bool")
					sc.SetScriptValue<bool>(offset, TryDeserailize(property, "Value", false));
				else if (type == "std::string")
					sc.SetScriptValue<std::string>(offset, TryDeserailize(property, "Value", std::string{}));
				else if (type == "glm::vec2")
					sc.SetScriptValue<glm::vec2>(offset, TryDeserailize(property, "Value", glm::vec2{}));
				else if (type == "glm::vec3")
					sc.SetScriptValue<glm::vec3>(offset, TryDeserailize(property, "Value", glm::vec3{}));
				else if (type == "glm::vec4")
					sc.SetScriptValue<glm::vec4>(offset, TryDeserailize(property, "Value", glm::vec4{}));
				else if (type == "glm::mat4")
					sc.SetScriptValue<glm::mat4>(offset, TryDeserailize(property, "Value", glm::mat4{}));
				else
					EK_CORE_WARN("Unknown type '{0}' for script property '{1}'", type, name);
			}
		}
	}
}