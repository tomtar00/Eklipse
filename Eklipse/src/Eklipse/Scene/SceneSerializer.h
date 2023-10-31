#pragma once
#include "Scene.h"
#include "Entity.h"

#include <yaml-cpp/yaml.h>
#include <filesystem>

namespace Eklipse 
{
	class SceneSerializer
	{
	public:
		SceneSerializer(const Ref<Scene> scene);

		bool Serialize(const std::filesystem::path& targetFilePath);
		bool Deserialize(const std::filesystem::path& sourceFilePath);

		void SerializeEntity(YAML::Emitter& out, Entity entity);
	
	private:
		Ref<Scene> m_scene;
	};
}