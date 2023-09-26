#include "EntitiesPanel.h"
#include "../EditorLayer.h"

#include <Eklipse/Scene/Components.h>

namespace Editor
{
	void EntitiesPanel::OnGUI()
	{
		EK_PROFILE();

		ImGui::Begin("Entities");
		ImVec2 hierarchySize = ImGui::GetContentRegionAvail();

		static Eklipse::Scene* scene = Eklipse::Application::Get().GetScene();
		scene->All([&](auto entityID)
			{
				Eklipse::Entity entity = { entityID, scene };
				auto& nameComponent = entity.GetComponent<Eklipse::NameComponent>();
				if (ImGui::Button(nameComponent.name.c_str(), { hierarchySize.x, 20 }))
				{
					EditorLayer::Get()->GetDetailsPanel().Setup(entity, nameComponent.name);
				}
			});

		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::MenuItem("New Entity"))
				scene->CreateEntity();
			if (ImGui::MenuItem("New Test Mesh")) // TODO: remove this later
				scene->CreateTestMesh("Test Mesh");

			ImGui::EndPopup();
		}

		ImGui::End();
	}
}