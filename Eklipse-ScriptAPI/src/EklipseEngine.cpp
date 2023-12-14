#include "precompiled.h"
#include "EklipseEngine.h"
#include "ScriptAPI.h"

#include <cstdarg>

namespace EklipseEngine
{
    class EntityImpl
    {
    public:
        EntityImpl(Eklipse::Entity* entity) : m_entity(*entity) {}

        template<typename T>
        bool HasComponent()
        {
            return m_entity->HasComponent<T>();
        }
        template<typename T, typename... Args>
        T& AddComponent(Args&&... args)
        {
            return m_entity->AddComponent<T>(std::forward<Args>(args)...);
        }
        template<typename T>
        T& GetComponent()
        {
            return m_entity->GetComponent<T>();
        }
        template<typename T>
        T* TryGetComponent()
        {
            return m_entity->TryGetComponent<T>();
        }
        template<typename T>
        void RemoveComponent()
        {
            m_entity->RemoveComponent<T>();
        }

        Eklipse::Entity m_entity;
    };

    Ref<Entity> Script::GetEntity()
    {
        return m_entity;
    }
    void Script::SetEntity(Ref<Entity> entity)
    {
        if (m_entity == nullptr)
		    m_entity = entity;
    }

    EK_API void Log(const char* message, ...)
    {		
        va_list args;
        va_start(args, message);
        ScriptAPI::Logger->info(message, args);
        va_end(args);
    }
    EK_API void LogWarn(const char* message, ...)
    {
        va_list args;
        va_start(args, message);
        ScriptAPI::Logger->warn(message, args);
        va_end(args);
    }
    EK_API void LogError(const char* message, ...)
    {
        va_list args;
        va_start(args, message);
        ScriptAPI::Logger->error(message, args);
        va_end(args);
    }
    
}