#include "EntitiesPanel.h"
#include "../EditorLayer.h"

namespace Editor
{
	void EntitiesPanel::OnGUI()
	{
		EK_PROFILE();

		ImGui::Begin("Entities");
		ImVec2 hierarchySize = ImGui::GetContentRegionAvail();

		auto& camera = Eklipse::Application::Get().GetScene()->m_camera;
		if (ImGui::Button(camera.m_name.c_str(), {hierarchySize.x, 20}))
		{
			EditorLayer::Get()->GetDetailsPanel().Setup(&camera);
		}
		for (auto& entity : Eklipse::Application::Get().GetScene()->m_entities)
		{
			if (ImGui::Button(entity.m_name.c_str(), { hierarchySize.x, 20 }))
			{
				EditorLayer::Get()->GetDetailsPanel().Setup(&entity);
			}
		}

		ImGui::End();
	}
}