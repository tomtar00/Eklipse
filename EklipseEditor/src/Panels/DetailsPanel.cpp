#include "DetailsPanel.h"
#include "EntitiesPanel.h"
#include <misc/cpp/imgui_stdlib.h>

namespace EklipseEditor
{
	void DetailsPanel::OnGUI()
	{
		ImGui::Begin("Details");
		if (g_selectedEntity == nullptr) 
		{
			ImGui::End(); 
			return;
		}

		std::string buff;
		if (!ImGui::InputText("EntityNameInput", &buff))
		{
			if (buff.size() > 0) g_selectedEntity->m_name = buff;
		}

		ImGui::End();
	}
}
