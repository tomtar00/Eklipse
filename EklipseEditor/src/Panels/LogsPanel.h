#pragma once
#include <Eklipse.h>

namespace Editor
{
	class LogsPanel : public Eklipse::GuiPanel
	{
		virtual void OnGUI(float deltaTime) override;
	};
}