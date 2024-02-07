#include "precompiled.h"
#include "Entity.h"
#include "Components.h"

namespace Eklipse
{
	UUID Entity::GetUUID()
	{
		return GetComponent<IDComponent>().ID;
	}
	String Entity::GetName()
	{
		return GetComponent<NameComponent>().name;
	}
	entt::entity Entity::GetHandle() const
	{
		return m_entityHandle;
	}

	bool Entity::IsNull() const
	{
        return m_entityHandle == entt::null;
    }
	void Entity::MarkNull()
	{
        m_entityHandle = entt::null;
        m_scene = nullptr;
    }

	bool Entity::operator==(const Entity& other) const 
	{
		return m_entityHandle == other.m_entityHandle && m_scene == other.m_scene;
	}
}