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
			EK_CORE_ERROR("Failed to load .hazel file '{0}'\n     {1}", sourceFilePath, e.what());
			return false;
		}

		if (!data["Scene"])
			return false;

		std::string sceneName = data["Scene"].as<std::string>();
		m_scene->SetName(sceneName);
		m_scene->SetPath(sourceFilePath);
		EK_CORE_TRACE("Deserializing scene '{0}'", sceneName);

		auto entities = data["Entities"];
		if (entities)
		{
			for (auto entity : entities)
			{
				uint64_t uuid = entity["Entity"].as<uint64_t>();

				std::string name;
				auto nameComponent = entity["NameComponent"];
				if (nameComponent)
					name = nameComponent["Name"].as<std::string>();

				EK_CORE_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid, name);

				Entity deserializedEntity = m_scene->CreateEntity(uuid, name);

				auto transformComponent = entity["TransformComponent"];
				if (transformComponent)
				{
					auto& tc = deserializedEntity.GetComponent<TransformComponent>();
					tc.transform.position = transformComponent["Position"].as<glm::vec3>();
					tc.transform.rotation = transformComponent["Rotation"].as<glm::vec3>();
					tc.transform.scale = transformComponent["Scale"].as<glm::vec3>();
				}

				auto cameraComponent = entity["CameraComponent"];
				if (cameraComponent)
				{
					auto& cc = deserializedEntity.AddComponent<CameraComponent>();
					auto& cameraProps = cameraComponent["Camera"];

					cc.camera.m_fov = (cameraProps["FOV"].as<float>());
					cc.camera.m_nearPlane = (cameraProps["Near"].as<float>());
					cc.camera.m_farPlane = (cameraProps["Far"].as<float>());
				}

				auto meshComponent = entity["MeshComponent"];
				if (meshComponent)
				{
					auto& mc = deserializedEntity.AddComponent<MeshComponent>();
					mc.meshPath = meshComponent["Mesh"].as<std::string>();
					mc.materialPath = meshComponent["Material"].as<std::string>();
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
			out << YAML::Key << "FOV" << YAML::Value << camera.m_fov;
			out << YAML::Key << "Near" << YAML::Value << camera.m_nearPlane;
			out << YAML::Key << "Far" << YAML::Value << camera.m_farPlane;
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

		out << YAML::EndMap;
	}
}