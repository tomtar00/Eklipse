#pragma once
#include <Eklipse.h>

namespace Eklipse
{
	class StatsPanel : public GuiPanel
	{
	public:
		virtual bool OnGUI(float deltaTime) override;		
	};
}