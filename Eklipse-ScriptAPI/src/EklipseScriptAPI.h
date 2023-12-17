#pragma once
#include "ScriptAPI/Reflections.h"

#include <Eklipse.h>
#include <Eklipse/Scene/Entity.h>

#include <spdlog/sinks/stdout_color_sinks.h>

namespace EklipseScriptAPI
{
	extern std::shared_ptr<spdlog::logger> Logger;

	EK_API struct LoggerConfig
	{
		std::string name;
		std::string pattern;
		spdlog::sink_ptr sink;
	};
	EK_API struct ScriptingConfig
	{
		LoggerConfig loggerConfig;
	};

	EK_API void Init(const ScriptingConfig& config);
}

namespace EklipseEngine
{
    class EK_API EntityImpl
    {
    public:
        EntityImpl(Eklipse::Entity entity) : m_entity(entity) {}

        template<typename T>
        bool HasComponent()
        {
            return m_entity->HasComponent<decltype(T::_impl::_comp)>();
        }
        template<typename T, typename... Args>
        T& AddComponent(Args&&... args)
        {
            return m_entity->AddComponent<decltype(T::_impl::_comp)>(std::forward<Args>(args)...);
        }
        template<typename T>
        Ref<decltype(T::_impl)> GetComponent()
        {
            return CreateRef<decltype(T::_impl)>(m_entity->GetComponent<decltype(T::_impl::_comp)>());
        }
        template<typename T>
        T* TryGetComponent()
        {
            return m_entity->TryGetComponent<decltype(T::_impl::_comp)>();
        }
        template<typename T>
        void RemoveComponent()
        {
            return m_entity->RemoveComponent<decltype(T::_impl::_comp)>();
        }

        Eklipse::Entity m_entity;
    };
}