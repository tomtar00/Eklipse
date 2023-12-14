#pragma once

namespace EklipseEngine
{
	template<typename T>
	using Unique = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Unique<T> CreateUnique(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	EK_API class Entity
	{
	public:
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

	private:
		class EntityImpl;
		Ref<EntityImpl> m_entity;
	};
	EK_API class Script
	{
	public:
		virtual void OnCreate() = 0;
		virtual void OnUpdate(float deltaTime) = 0;

		Ref<Entity> GetEntity();
		void SetEntity(Ref<Entity> entity);

	private:
		Ref<Entity> m_entity;
	};

	EK_API void Log(const char* message, ...);
	EK_API void LogWarn(const char* message, ...);
	EK_API void LogError(const char* message, ...);
}