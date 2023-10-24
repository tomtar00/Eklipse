#pragma once
#include <Eklipse.h>

namespace Editor
{
	class SettingsPanel : public Eklipse::GuiPanel
	{
		virtual void OnGUI(float deltaTime) override;
	};
}