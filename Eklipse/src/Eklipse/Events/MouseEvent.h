#pragma once

#include "Event.h"
#include <string>
#include <sstream>

namespace Eklipse 
{
	class MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(float x, float y)
			: m_mouseX(x), m_mouseY(y) {}

		inline float GetX() const { return m_mouseX; }
		inline float GetY() const { return m_mouseY; }

		String ToString() const override
		{
			std::stringstream ss;
			ss << "MouseMovedEvent: " << m_mouseX << ", " << m_mouseY;
			return ss.str();
		}

		static EventType GetStaticType() { return EventType::MouseMoved; }
		virtual EventType GetEventType() const override { return GetStaticType(); }

		virtual int GetCategoryFlags() const override { return EventCategoryMouse | EventCategoryInput; }
	private:
		float m_mouseX, m_mouseY;
	};

	class MouseScrolledEvent : public Event
	{
	public:
		MouseScrolledEvent(float xOffset, float yOffset)
			: m_xOffset(xOffset), m_yOffset(yOffset) {}

		inline float GetXOffset() const { return m_xOffset; }
		inline float GetYOffset() const { return m_yOffset; }

		String ToString() const override
		{
			std::stringstream ss;
			ss << "MouseScrolledEvent: " << GetXOffset() << ", " << GetYOffset();
			return ss.str();
		}

		static EventType GetStaticType() { return EventType::MouseScrolled; }
		virtual EventType GetEventType() const override { return GetStaticType(); }

		virtual int GetCategoryFlags() const override { return EventCategoryMouse | EventCategoryInput; }
	private:
		float m_xOffset, m_yOffset;
	};

	class MouseButtonEvent : public Event
	{
	public:
		inline int GetMouseButton() const { return m_button; }

		virtual int GetCategoryFlags() const override { return EventCategoryMouse | EventCategoryInput; }
	protected:
		MouseButtonEvent(int button)
			: m_button(button) {}

		int m_button;
	};

	class MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(int button)
			: MouseButtonEvent(button) {}

		String ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: " << m_button;
			return ss.str();
		}

		static EventType GetStaticType() { return EventType::MouseButtonPressed; }
		virtual EventType GetEventType() const override { return GetStaticType(); }
	};

	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(int button)
			: MouseButtonEvent(button) {}

		String ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent: " << m_button;
			return ss.str();
		}

		static EventType GetStaticType() { return EventType::MouseButtonReleased; }
		virtual EventType GetEventType() const override { return GetStaticType(); }
	};

}