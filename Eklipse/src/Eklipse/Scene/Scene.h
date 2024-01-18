#pragma once
#include <entt/entt.hpp>
#include <Eklipse/Core/UUID.h>
#include <Eklipse/Utils/File.h>

#include <Eklipse/Scene/Camera.h>
#include <Eklipse/Scene/Transform.h>

class dylib;

namespace Eklipse
{
	class Entity;

	class EK_API Scene
	{
	public:
		Scene();
		Scene(const std::string& name, const std::filesystem::path& saveFilePath);
		~Scene();

		void ApplyAllComponents();

		void OnSceneStart();
		void OnSceneUpdate(float deltaTime);
		void OnSceneStop();
		
		static Ref<Scene> Copy(Ref<Scene> other);
		static Ref<Scene> New(const std::string& name, const std::filesystem::path& saveFilePath);
		static Ref<Scene> Load(const std::filesystem::path& saveFilePath, const Ref<dylib>& library);
		static void Save(Ref<Scene> scene);
		static void ReloadScripts(Ref<Scene> scene);

		Entity CreateEntity(const std::string name = "");
		Entity CreateEntity(UUID uuid, const std::string& name = "");
		Entity GetEntity(UUID uuid);
		void DestroyEntity(Entity entity);

		Camera* GetMainCamera() { return m_mainCamera; }
		Transform* GetMainCameraTransform() { return m_mainCameraTransform; }

		template<typename Func>
		inline void ForEachEntity(Func& func) { m_registry.each(func); }
		entt::registry& GetRegistry() { return m_registry; }
		const std::string& GetName() const { return m_name; }
		const Path& GetPath() const { return m_path; }

		void SetName(const std::string& name) { m_name = name; }
		void SetPath(const Path& path) { m_path = path; }

	private:
		std::string m_name;
		Path m_path;

		entt::registry m_registry;
		std::unordered_map<UUID, entt::entity> m_entityMap;

		Camera* m_mainCamera = nullptr;
		Transform* m_mainCameraTransform = nullptr;
	};
}