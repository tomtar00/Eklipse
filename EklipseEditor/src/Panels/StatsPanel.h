#pragma once
#include <Eklipse.h>

namespace Editor
{
	class StatsPanel : public Eklipse::GuiPanel
	{
		virtual void OnGUI(float deltaTime) override;
	};
}