#pragma once
#include <Eklipse.h>

namespace Editor
{
	class SettingsPanel : public Eklipse::GuiPanel
	{
		virtual bool OnGUI(float deltaTime) override;
	};
}