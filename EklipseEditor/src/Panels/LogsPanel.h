#pragma once
#include <Eklipse.h>

namespace Editor
{
	class LogsPanel : public Eklipse::Layer
	{
		virtual void OnGUI(float deltaTime) override;
	};
}