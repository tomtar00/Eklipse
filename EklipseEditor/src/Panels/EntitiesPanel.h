#pragma once
#include <Eklipse.h>

namespace Editor
{
	class EntitiesPanel : public Eklipse::Layer
	{
		virtual void OnGUI(float deltaTime) override;
	};
}