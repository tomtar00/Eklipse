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
		static Ref<Scene> New(const String& name);
		static Ref<Scene> Copy(Scene* other);
		static Ref<Scene> Load(const Path& filePath, const AssetHandle handle = AssetHandle());
		static void Save(Ref<Scene> scene, const Path& filePath);

		// Scripts
		void DestroyAllScripts();
		void InitializeAllScripts(const Path& filePath);

		// Components
		void ApplyAllComponents();

		// Entity
		Entity CreateEntity(const String& name = "");
		Entity CreateEntity(UUID uuid, const String& name = "");
		Entity GetEntity(UUID uuid);
		void DestroyEntity(Entity entity);	

		// Serialization
		bool Serialize(const Path& filePath);
		bool SerializeEntity(Entity entity, YAML::Emitter& out);
		bool Deserialize(const Path& filePath);
		bool DeserializeEntity(YAML::Node& entityNode);
		bool DeserializeEveryScriptProperties(const Path& filePath);
		bool DeserializeScriptProperties(Entity entity, YAML::Node& propertiesNode);

		// Getters
		Camera* GetMainCamera() { return m_mainCamera; }
		Transform* GetMainCameraTransform() { return m_mainCameraTransform; }
		entt::registry& GetRegistry() { return m_registry; }

		// Helpers
		template<typename Func>
		inline void ForEachEntity(Func& func) { m_registry.each(func); }

		// Asset
		virtual void Dispose() override;
		static AssetType GetStaticType() { return AssetType::Scene; }
		virtual AssetType GetType() const override { return GetStaticType(); }

	private:
		entt::registry m_registry;
		std::unordered_map<UUID, entt::entity> m_entityMap;

		Camera* m_mainCamera = nullptr;
		Transform* m_mainCameraTransform = nullptr;
	};
}