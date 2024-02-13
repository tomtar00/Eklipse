#include "precompiled.h"
#include <ScriptAPI/Input.h>
#include <Eklipse/Core/Input.h>

namespace EklipseEngine
{
	bool Input::IsKeyDown(KeyCode key)
	{
		return Eklipse::Input::IsKeyDown((Eklipse::KeyCode)(int)key);
	}
	bool Input::IsMouseButtonDown(MouseCode button)
	{
		return Eklipse::Input::IsMouseButtonDown((Eklipse::MouseCode)(int)button);
	}
	glm::vec2 Input::GetMousePosition()
	{
		return Eklipse::Input::GetMousePosition();
	}
	glm::vec2 Input::GetMouseDelta()
	{
		return Eklipse::Input::GetMouseDelta();
	}
	float Input::GetScrollValue()
	{
		return Eklipse::Input::GetScrollValue();
	}
	bool Input::IsScrollingUp()
	{
		return Eklipse::Input::IsScrollingUp();
	}
	bool Input::IsScrollingDown()
	{
		return Eklipse::Input::IsScrollingDown();
	}
}