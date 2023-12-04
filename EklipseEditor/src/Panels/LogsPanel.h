#pragma once
#include <Eklipse.h>

namespace Editor
{
	class LogsPanel : public Eklipse::GuiPanel
	{
		virtual bool OnGUI(float deltaTime) override;
	};
}