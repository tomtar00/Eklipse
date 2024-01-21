#include "precompiled.h"
#include "Entity.h"
#include "Components.h"

namespace Eklipse
{
	UUID Entity::GetUUID()
	{
		return GetComponent<IDComponent>().ID;
	}
	std::string Entity::GetName()
	{
		return GetComponent<NameComponent>().name;
	}
	entt::entity Entity::GetHandle() const
	{
		return m_entityHandle;
	}
}