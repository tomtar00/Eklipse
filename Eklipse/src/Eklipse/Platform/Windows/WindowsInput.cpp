#include <precompiled.h>

#include "Eklipse/Core/Input.h"
#include "Eklipse/Core/Application.h"
#include <GLFW/glfw3.h>

namespace Eklipse
{
	bool Input::IsKeyDown(const KeyCode key)
	{
		auto* window = Application::Get().GetWindow()->GetGlfwWindow();
		auto state = glfwGetKey(window, static_cast<int32_t>(key));
		return state == GLFW_PRESS;
	}

	bool Input::IsMouseButtonDown(const MouseCode button)
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
}