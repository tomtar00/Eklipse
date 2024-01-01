#pragma once
#include <Eklipse.h>

namespace Eklipse
{
	class SettingsPanel : public GuiPanel
	{
		virtual bool OnGUI(float deltaTime) override;
	};
}