#pragma once
#include <Eklipse.h>

namespace EklipseEditor
{
	extern Eklipse::Entity* g_selectedEntity;

	class EntitiesPanel : public Eklipse::ImGuiPanel
	{
		void OnGUI();
	};
}