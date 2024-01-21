#pragma once
#include <entt/entt.hpp>
#include <Eklipse/Core/UUID.h>
#include <Eklipse/Utils/File.h>

#include <Eklipse/Scene/Camera.h>
#include <Eklipse/Scene/Transform.h>

#include <Eklipse/Utils/Yaml.h>

class dylib;

namespace Eklipse
{
	class Entity;

	enum class SceneState
	{
		NONE	= BIT(1),
		RUNNING = BIT(2),
		PAUSED	= BIT(3),
	};

	class EK_API Scene
	{
	public:
		Scene();
		Scene(const std::string& name, const std::filesystem::path& saveFilePath);
		~Scene();

		// Scene Events
		void OnSceneStart();
		void OnSceneUpdate(float deltaTime);
		void OnScenePause();
		void OnSceneResume();
		void OnSceneStop();
		
		// Scene
		static Ref<Scene> Copy(Ref<Scene> other);
		static Ref<Scene> New(const std::string& name, const std::filesystem::path& saveFilePath);
		static Ref<Scene> Load(const std::filesystem::path& saveFilePath, const Ref<dylib>& library);
		static void Save(Ref<Scene> scene);

		// Scripts
		void DestroyAllScripts();
		void InitializeAllScripts();

		// Components
		void ApplyAllComponents();

		// Entity
		Entity CreateEntity(const std::string name = "");
		Entity CreateEntity(UUID uuid, const std::string& name = "");
		Entity GetEntity(UUID uuid);
		void DestroyEntity(Entity entity);	

		// Serialization
		bool Serialize();
		bool SerializeEntity(Entity entity, YAML::Emitter& out);
		bool Deserialize(const Ref<dylib>& library);
		bool DeserializeEntity(YAML::Node& entityNode, const Ref<dylib>& library);
		bool DeserializeEveryScriptProperties();
		bool DeserializeScriptProperties(Entity entity, YAML::Node& propertiesNode);

		// Getters
		Camera* GetMainCamera() { return m_mainCamera; }
		Transform* GetMainCameraTransform() { return m_mainCameraTransform; }
		entt::registry& GetRegistry() { return m_registry; }
		const std::string& GetName() const { return m_name; }
		const Path& GetPath() const { return m_path; }
		SceneState GetState() const { return m_state; }

		// Setters
		void SetName(const std::string& name) { m_name = name; }
		void SetPath(const Path& path) { m_path = path; }

		// Helpers
		template<typename Func>
		inline void ForEachEntity(Func& func) { m_registry.each(func); }

	private:
		std::string m_name;
		Path m_path;
		SceneState m_state;

		entt::registry m_registry;
		std::unordered_map<UUID, entt::entity> m_entityMap;

		Camera* m_mainCamera = nullptr;
		Transform* m_mainCameraTransform = nullptr;
	};
}