#pragma once
#include "Core.h"
#include "Entity.h"	

namespace EklipseEngine
{
	class EK_SCRIPT_API Script
	{
	public:
		Script() = default;
		virtual ~Script() = default;

		virtual void OnCreate() = 0;
		virtual void OnUpdate(float deltaTime) = 0;

		void SetEntity(Ref<Eklipse::Entity> entity);

		template<typename T>
		bool HasComponent() {
			return m_entity->HasComponent<T>();
		}

		template<typename T, typename... Args>
		T AddComponent(Args&&... args) {
			return m_entity->AddComponent<T>(std::forward<Args>(args)...);
		}

		template<typename T>
		T GetComponent() {
			return m_entity->GetComponent<T>();
		}

		template<typename T>
		void RemoveComponent() {
			m_entity->RemoveComponent<T>();
		}

	private:
		Ref<Entity> m_entity;
	};
}