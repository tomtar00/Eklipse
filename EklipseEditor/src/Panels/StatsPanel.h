#pragma once
#include <Eklipse.h>

namespace Editor
{
	class StatsPanel : public Eklipse::Layer
	{
		virtual void OnGUI(float deltaTime) override;
	};
}