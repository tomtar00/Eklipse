#pragma once
#include <Eklipse.h>

namespace Editor
{
	class EntitiesPanel : public Eklipse::GuiPanel
	{
		virtual void OnGUI(float deltaTime) override;
	};
}