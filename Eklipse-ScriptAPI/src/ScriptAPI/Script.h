#pragma once
#include "Core.h"

namespace EklipseEngine
{
	class EntityImpl
	{
		template<typename T>
		bool HasComponent();
		template<typename T, typename... Args>
		T& AddComponent(Args&&... args);
		template<typename T>
		Ref<T> GetComponent();
		template<typename T>
		T* TryGetComponent();
		template<typename T>
		void RemoveComponent();
	};
	class EK_API Entity
	{
	public:
		Entity() = delete;
		Entity(Ref<EntityImpl> entity) : m_entity(entity) {}

		template<typename T>
		bool HasComponent()
		{
			return m_entity->HasComponent<T>();
		}
		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			return m_entity->AddComponent<T>(std::forward<Args>(args)...);
		}
		template<typename T>
		Ref<T> GetComponent()
		{
			return CreateRef<T>(m_entity->GetComponent<T>());
		}
		template<typename T>
		T* TryGetComponent()
		{
			return m_entity->TryGetComponent<T>();
		}
		template<typename T>
		void RemoveComponent()
		{
			m_entity->RemoveComponent<T>();
		}

	private:
		Ref<EntityImpl> m_entity;
	};

	class EK_API Script
	{
	public:
		virtual void OnCreate() = 0;
		virtual void OnUpdate(float deltaTime) = 0;

		Ref<Entity> GetEntity();
		void SetEntity(Ref<EntityImpl> entity);

	private:
		Ref<Entity> m_entity;
	};
}