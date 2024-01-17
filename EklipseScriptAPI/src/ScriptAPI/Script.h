#pragma once
#include "Core.h"

namespace Eklipse
{
	class Entity;
}

namespace EklipseEngine
{
	class EK_API Entity
	{
	public:
		Entity() = delete;
		Entity(Ref<Eklipse::Entity> entity);

		template<typename T>
		bool HasComponent();

		template<typename T, typename... Args>
		T AddComponent(Args&&... args);

		template<typename T>
		T GetComponent();

		template<typename T>
		void RemoveComponent();

	private:
		Ref<Eklipse::Entity> m_entity;
	};

	class EK_API Script
	{
	public:
		virtual void OnCreate() = 0;
		virtual void OnUpdate(float deltaTime) = 0;

		Ref<Entity> GetEntity();
		void SetEntity(Ref<Eklipse::Entity> entity);

	private:
		Ref<Entity> m_entity;
	};
}