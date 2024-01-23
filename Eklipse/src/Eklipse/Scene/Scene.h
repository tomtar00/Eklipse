#pragma once
#include <entt/entt.hpp>
#include <Eklipse/Core/UUID.h>
#include <Eklipse/Utils/File.h>
#include <Eklipse/Scene/Camera.h>
#include <Eklipse/Scene/Transform.h>
#include <Eklipse/Utils/Yaml.h>
#include <Eklipse/Assets/Asset.h>

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

	class EK_API Scene : public Asset
	{
	public:
		Scene();
		~Scene();

		// Scene Events
		void OnSceneStart();
		void OnSceneUpdate(float deltaTime);
		void OnScenePause();
		void OnSceneResume();
		void OnSceneStop();
		
		// Scene
		static Ref<Scene> New();
		static Ref<Scene> Copy(Ref<Scene> other);
		static Ref<Scene> Load(const std::filesystem::path& filePath);
		static void Save(Ref<Scene> scene, const std::filesystem::path& filePath);

		// Scripts
		void DestroyAllScripts();
		void InitializeAllScripts();

		// Components
		void ApplyAllComponents();

		// Entity
		Entity CreateEntity(const std::string& name = "");
		Entity CreateEntity(UUID uuid, const std::string& name = "");
		Entity GetEntity(UUID uuid);
		void DestroyEntity(Entity entity);	

		// Serialization
		bool Serialize(const std::filesystem::path& filePath);
		bool SerializeEntity(Entity entity, YAML::Emitter& out);
		bool Deserialize(const std::filesystem::path& filePath);
		bool DeserializeEntity(YAML::Node& entityNode);
		bool DeserializeEveryScriptProperties(const std::filesystem::path& filePath);
		bool DeserializeScriptProperties(Entity entity, YAML::Node& propertiesNode);

		// Getters
		Camera* GetMainCamera() { return m_mainCamera; }
		Transform* GetMainCameraTransform() { return m_mainCameraTransform; }
		entt::registry& GetRegistry() { return m_registry; }
		const std::string& GetName() const { return m_name; }
		SceneState GetState() const { return m_state; }

		// Helpers
		template<typename Func>
		inline void ForEachEntity(Func& func) { m_registry.each(func); }

		// Asset
		static AssetType GetStaticType() { return AssetType::Scene; }
		virtual AssetType GetType() const override { return GetStaticType(); }

	private:
		std::string m_name;
		SceneState m_state;

		entt::registry m_registry;
		std::unordered_map<UUID, entt::entity> m_entityMap;

		Camera* m_mainCamera = nullptr;
		Transform* m_mainCameraTransform = nullptr;
	};
}