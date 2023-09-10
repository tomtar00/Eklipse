#pragma once
#include <Eklipse.h>

namespace Editor
{
	class ViewPanel : public Eklipse::ImGuiPanel
	{
		void OnGUI();
	};
}