#pragma once
#include <Eklipse/Scene/Entity.h>

namespace Eklipse
{
	class Script
	{
	public:
		virtual void OnCreate() = 0;
		virtual void OnUpdate(float deltaTime) = 0;

	public:
		template<typename T>
		T& GetComponent()
		{
			return m_entity.GetComponent<T>();
		}
		void SetEntity(Eklipse::Entity entity);

	private:
		Eklipse::Entity m_entity;
	};
}