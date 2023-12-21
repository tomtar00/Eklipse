#pragma once
#include "Scene.h"
#include "Entity.h"

#include <yaml-cpp/yaml.h>

namespace Eklipse 
{
	class SceneSerializer
	{
	public:
		SceneSerializer(const Ref<Scene> scene);

		bool Serialize(const Path& targetFilePath);
		bool Deserialize(const Path& sourceFilePath);

		void SerializeEntity(YAML::Emitter& out, Entity entity);
		void DeserializeAllScriptProperties();

	private:
		void DeserializeScriptProperties(Entity entity, YAML::Node& propertiesNode);
	
	private:
		Ref<Scene> m_scene;
	};
}