#pragma once
#include <Eklipse.h>

namespace EklipseEditor
{
	class ViewPanel : public Eklipse::ImGuiPanel
	{
		void OnGUI();
	};
}