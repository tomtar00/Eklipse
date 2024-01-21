#include "precompiled.h"
#include <ScriptAPI/Script.h>

namespace EklipseEngine
{
    void Script::SetEntity(Ref<Eklipse::Entity> entity)
    {
        if (m_entity == nullptr)
            m_entity = EklipseEngine::CreateRef<Entity>(entity);
    }
}