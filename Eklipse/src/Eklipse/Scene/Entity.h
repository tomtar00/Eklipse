#pragma once

#include <entt.hpp>
#include "Scene.h"

namespace Eklipse
{
	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity handle, Scene* scene) : m_entityHandle(handle), m_scene(scene) {}
		
		template<typename T>
		bool HasComponent()
		{
			return m_scene->GetRegistry().all_of<T>(m_entityHandle);
		}
		template<typename T, typename... Args>
		void AddComponent(Args&&... args)
		{
			EK_ASSERT(!HasComponent<T>(), "Entity already has component!");
			m_scene->GetRegistry().emplace<T>(m_entityHandle, std::forward<Args>(args)...);
		}
		template<typename T>
		T& GetComponent()
		{
			EK_ASSERT(HasComponent<T>(), "Entity does not have component!");
			return m_scene->GetRegistry().get<T>(m_entityHandle);
		}
		template<typename T>
		T* TryGetComponent()
		{
			return m_scene->GetRegistry().try_get<T>(m_entityHandle);
		}
		template<typename T>
		void RemoveComponent()
		{
			EK_ASSERT(HasComponent<T>(), "Entity does not have component!");
			m_scene->GetRegistry().remove<T>(m_entityHandle);
		}
		
		inline bool IsNull() const { return m_entityHandle == entt::null; }
		inline void MarkNull() { m_entityHandle = entt::null; }
		entt::entity m_entityHandle{ entt::null };
	
	private:
		Scene* m_scene = nullptr;
	};
}