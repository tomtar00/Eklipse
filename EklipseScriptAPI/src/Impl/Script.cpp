#include "precompiled.h"
#include <ScriptAPI/Script.h>

namespace EklipseEngine
{
    Ref<Entity> Script::GetEntity()
    {
        return m_entity;
    }
    void Script::SetEntity(Ref<Eklipse::Entity> entity)
    {
        if (m_entity == nullptr)
            m_entity = EklipseEngine::CreateRef<Entity>(entity);
    }
}