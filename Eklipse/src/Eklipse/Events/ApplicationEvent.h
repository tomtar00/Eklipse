#pragma once
#include "precompiled.h"
#include "Event.h"

namespace Eklipse 
{
	class EK_API WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(unsigned int width, unsigned int height)
			: m_width(width), m_height(height) {}

		inline unsigned int GetWidth() const { return m_width; }
		inline unsigned int GetHeight() const { return m_height; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: " << m_width << ", " << m_height;
			return ss.str();
		}

		static EventType GetStaticType() { return EventType::WindowResize; }
		virtual EventType GetEventType() const override { return GetStaticType(); }

		virtual int GetCategoryFlags() const override { return EventCategoryApplication; }
	private:
		unsigned int m_width, m_height;
	};

	/*class EK_API FramebufferResizeEvent : public Event
	{
	public:
		FramebufferResizeEvent(unsigned int width, unsigned int height)
			: m_width(width), m_height(height) {}

		inline unsigned int GetWidth() const { return m_width; }
		inline unsigned int GetHeight() const { return m_height; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "FramebufferResizeEvent: " << m_width << ", " << m_height;
			return ss.str();
		}

		static EventType GetStaticType() { return EventType::FramebufferResize; }
		virtual EventType GetEventType() const override { return GetStaticType(); }

		virtual int GetCategoryFlags() const override { return EventCategoryApplication; }
	private:
		unsigned int m_width, m_height;
	};*/

	class EK_API WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() {}

		static EventType GetStaticType() { return EventType::WindowClose; }
		virtual EventType GetEventType() const override { return GetStaticType(); }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowCloseEvent";
			return ss.str();
		}

		virtual int GetCategoryFlags() const override { return EventCategoryApplication; }
	};

	/*class EK_API WindowMinimizedEvent : public Event
	{
	public:
		WindowMinimizedEvent() {}

		static EventType GetStaticType() { return EventType::WindowMiminized; }
		virtual EventType GetEventType() const override { return GetStaticType(); }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowMinimizedEvent";
			return ss.str();
		}

		virtual int GetCategoryFlags() const override { return EventCategoryApplication; }
	};

	class EK_API WindowMaximizedEvent : public Event
	{
	public:
		WindowMaximizedEvent() {}

		static EventType GetStaticType() { return EventType::WindowMaximized; }
		virtual EventType GetEventType() const override { return GetStaticType(); }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowMaximizedEvent";
			return ss.str();
		}

		virtual int GetCategoryFlags() const override { return EventCategoryApplication; }
	};*/
}