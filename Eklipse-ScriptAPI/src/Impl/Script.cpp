#include "precompiled.h"
#include <ScriptAPI/Script.h>
#include <Eklipse/Scene/Entity.h>

#include <Decl/Components.h>
#include <ScriptAPI/Components.h>
#include <Eklipse/Scene/Components.h>

namespace EklipseEngine
{
    Entity::Entity(Ref<Eklipse::Entity> entity) : m_entity(entity) {}

    Ref<Entity> Script::GetEntity()
    {
        return m_entity;
    }
    void Script::SetEntity(Ref<Eklipse::Entity> entity)
    {
        if (m_entity == nullptr)
            m_entity = EklipseEngine::CreateRef<Entity>(entity);
    }

    template<typename T>
    bool Entity::HasComponent()
    {
        return m_entity->HasComponent<T::Comp>();
    }
    template<typename T, typename... Args>
    T Entity::AddComponent(Args&&... args)
    {
        return T(EklipseEngine::CreateRef<T::Impl>(m_entity->AddComponent<T::Comp>(std::forward<Args>(args)...)));
    }
    template<typename T>
    T Entity::GetComponent()
    {
        return T(EklipseEngine::CreateRef<T::Impl>(m_entity->GetComponent<T::Comp>()));
    }
    template<typename T>
    void Entity::RemoveComponent()
    {
        m_entity->RemoveComponent<T::Comp>();
    }

    EK_CREATE_EXPLICIT_ENTITY_TEMPLATES(EklipseEngine::Transform);
}