#pragma once
#include <Eklipse.h>

namespace Editor
{
	class StatsPanel : public Eklipse::GuiPanel
	{
	public:
		virtual bool OnGUI(float deltaTime) override;		
	};
}