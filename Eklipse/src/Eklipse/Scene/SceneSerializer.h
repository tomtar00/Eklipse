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

		bool Serialize(const std::filesystem::path& targetFilePath);
		bool Deserialize(const std::filesystem::path& sourceFilePath, const Ref<dylib>& library);

		void SerializeEntity(YAML::Emitter& out, Entity entity);
		void DeserializeAllScriptProperties();

	private:
		void DeserializeScriptProperties(Entity entity, YAML::Node& propertiesNode);
	
	private:
		Ref<Scene> m_scene;
	};
}