#pragma once
#include <entt.hpp>
#include <Eklipse/Core/UUID.h>
#include <Eklipse/Utils/File.h>

namespace Eklipse
{
	class Entity;

	class Scene
	{
	public:
		Scene() = default;
		Scene(const std::string& name, const Path& saveFilePath);

		void Unload();

		void ApplyAllComponents();
		
		static Ref<Scene> New(const std::string& name, const Path& saveFilePath);
		static void Save(Ref<Scene> scene);
		static Ref<Scene> Load(const Path& saveFilePath);

		Entity CreateEntity(const std::string name = "");
		Entity CreateEntity(UUID uuid, const std::string& name = "");
		void DestroyEntity(Entity entity);

		template<typename Func>
		inline void ForEachEntity(Func& func) { m_registry.each(func); }

		entt::registry& GetRegistry() { return m_registry; }
		const std::string& GetName() const { return m_name; }
		const Path& GetPath() const { return m_path; }

		void SetName(const std::string& name) { m_name = name; }
		void SetPath(const Path& path) { m_path = path; }

	private:
		entt::registry m_registry;

		std::string m_name;
		Path m_path;
	};
}