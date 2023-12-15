#include "precompiled.h"
#include <ScriptAPI/Script.h>

namespace EklipseEngine
{
    Ref<Entity> Script::GetEntity()
    {
        return m_entity;
    }
    void Script::SetEntity(Ref<EntityImpl> entity)
    {
        if (m_entity == nullptr)
            m_entity = CreateRef<Entity>(entity);
    }

    /*template<typename T>
    bool Entity::HasComponent()
    {
        return m_entity->HasComponent<T>();
    }
    template<typename T, typename... Args>
    T& Entity::AddComponent(Args&&... args)
    {
        return m_entity->AddComponent<T>(std::forward<Args>(args)...);
    }
    template<typename T>
    Ref<T> Entity::GetComponent()
    {
        return CreateRef<T>(m_entity->GetComponent<T>());
    }
    template<typename T>
    T* Entity::TryGetComponent()
    {
        return m_entity->TryGetComponent<T>();
    }
    template<typename T>
    void Entity::RemoveComponent()
    {
        m_entity->RemoveComponent<T>();
    }*/
}