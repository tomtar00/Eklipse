#include "EntitiesPanel.h"

namespace EklipseEditor
{
	void EntitiesPanel::OnGUI()
	{
		ImGui::Begin("Hierarchy");
		ImGui::Text("All scene entities are here");
		ImGui::End();
	}
}