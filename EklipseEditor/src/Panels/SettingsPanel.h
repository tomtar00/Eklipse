#pragma once
#include <Eklipse.h>

namespace Editor
{
	class SettingsPanel : public Eklipse::Layer
	{
		virtual void OnGUI(float deltaTime) override;
	};
}