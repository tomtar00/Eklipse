#include "EntitiesPanel.h"

namespace EklipseEditor
{
	Eklipse::Entity* g_selectedEntity = nullptr;

	void EntitiesPanel::OnGUI()
	{
		ImGui::Begin("Hierarchy");
		ImVec2 hierarchySize = ImGui::GetContentRegionAvail();
		uint32_t elementIdCounter = 0;

		for (auto& entity : Eklipse::Application::Get().GetScene()->m_entities)
		{
			if (ImGui::Button(entity.m_name.c_str(), {hierarchySize.x, 20}))
			{
				g_selectedEntity = &entity;
			}
		}

		ImGui::End();
	}
}