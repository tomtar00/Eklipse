#pragma once
#include <Eklipse.h>

namespace Editor
{
	class EntitiesPanel : public Eklipse::GuiPanel
	{
		virtual bool OnGUI(float deltaTime) override;
	};
}