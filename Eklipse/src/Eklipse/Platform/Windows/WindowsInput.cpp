#include <precompiled.h>

#include "Eklipse/Core/Input.h"
#include "Eklipse/Core/Application.h"
#include <GLFW/glfw3.h>

namespace Eklipse
{
	glm::vec2 Input::m_mousePosition = { 0.0f, 0.0f };
	glm::vec2 Input::m_mouseScrollDelta = { 0.0f, 0.0f };

	bool Input::IsKeyDown(KeyCode key)
	{
		auto* window = Application::Get().GetWindow()->GetGlfwWindow();
		auto state = glfwGetKey(window, static_cast<int32_t>(key));
		return state == GLFW_PRESS;
	}
	bool Input::IsMouseButtonDown(MouseCode button)
	{
		auto* window = Application::Get().GetWindow()->GetGlfwWindow();
		auto state = glfwGetMouseButton(window, static_cast<int32_t>(button));
		return state == GLFW_PRESS;
	}
	glm::vec2 Input::GetMousePosition()
	{
		auto* window = Application::Get().GetWindow()->GetGlfwWindow();
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		return { (float)xpos, (float)ypos };
	}
	float Input::GetMouseDeltaX()
	{
		return m_mousePosition.x - GetMousePosition().x;
	}
	float Input::GetMouseDeltaY()
	{
		return m_mousePosition.y - GetMousePosition().y;
	}
	float Input::GetScrollValue()
	{
		return m_mouseScrollDelta.y;
	}
	bool Input::IsScrollingUp()
	{
		return m_mouseScrollDelta.y > 0;
	}
	bool Input::IsScrollingDown()
	{
		return m_mouseScrollDelta.y < 0;
	}
	void Input::Reset()
	{
		m_mouseScrollDelta = { 0.f, 0.f };
	}
}