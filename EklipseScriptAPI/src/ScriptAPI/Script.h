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

		Ref<Entity> GetEntity();
		void SetEntity(Ref<Eklipse::Entity> entity);

	private:
		Ref<Entity> m_entity;
	};
}