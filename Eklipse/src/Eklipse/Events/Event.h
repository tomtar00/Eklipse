#pragma once

#include "Eklipse/Core/Core.h"

namespace Eklipse 
{
	enum class EventType
	{
		None = 0,
		WindowClose, WindowMiminized, WindowMaximized, WindowResize, WindowFocus, WindowLostFocus,
		FramebufferResize,
		KeyPressed, KeyReleased,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	enum EventCategory
	{
		None = 0,
		EventCategoryApplication	= BIT(0),
		EventCategoryInput			= BIT(1),
		EventCategoryKeyboard		= BIT(2),
		EventCategoryMouse			= BIT(3),
		EventCategoryMouseButton	= BIT(4)
	};

	class EK_API Event
	{
	public:
		virtual ~Event() = default;

		bool Handled = false;

		virtual EventType GetEventType() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual String ToString() const { return typeid(this).name(); }

		bool IsInCategory(EventCategory category) const
		{
			return GetCategoryFlags() & category;
		}
	};

	class EK_API EventDispatcher
	{
	public:
		EventDispatcher(Event& event)
			: m_event(event) {}

		template<typename T, typename F>
		bool Dispatch(const F& func)
		{
			if (m_event.Handled) return false;

			if (m_event.GetEventType() == T::GetStaticType())
			{
				m_event.Handled = true;
				func(static_cast<T&>(m_event));
			}
			return m_event.Handled;
		}
	private:
		Event& m_event;
	};
}
