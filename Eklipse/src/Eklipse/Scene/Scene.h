#pragma once
#include <entt.hpp>

namespace Eklipse
{
	class Entity;

	class Scene
	{
	public:
		void Load();
		void Dispose();

		//
		void CreateTestMesh(const std::string& name); // TODO: remove this
		//

		Entity CreateEntity(const std::string name = "");
		void DestroyEntity(Entity entity);

		template<typename Func>
		inline void All(Func& func) { GetRegistry().each(func); }

		entt::registry& GetRegistry() { return m_registry; }

	private:
		entt::registry m_registry;
	};
}