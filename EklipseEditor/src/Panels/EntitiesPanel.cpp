#include "EntitiesPanel.h"
#include "../EditorLayer.h"

#include <Eklipse/Scene/Components.h>

namespace Editor
{
	void EntitiesPanel::OnGUI(float deltaTime)
	{
		EK_PROFILE();

		ImGui::Begin("Entities");
		static Eklipse::Scene* scene = EditorLayer::Get()->GetActiveScene();
		EK_ASSERT(scene != nullptr, "Scene is null!");

		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::MenuItem("New Entity"))
				scene->CreateEntity();
			if (ImGui::MenuItem("New Test Mesh")) // TODO: remove this later
				scene->CreateTestMesh("Test Mesh");

			ImGui::EndPopup();
		}

		scene->ForEachEntity([&](auto entityID)
			{
				Eklipse::Entity entity = { entityID, scene };
				auto& nameComponent = entity.GetComponent<Eklipse::NameComponent>();

				bool expand = ImGui::TreeNodeEx(nameComponent.name.c_str(), ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth);
				
				if (ImGui::IsItemClicked())
				{
					EditorLayer::Get()->SetSelectedEntity(entity);
					EditorLayer::Get()->GetDetailsPanel().Setup(nameComponent.name);
				}

				if (ImGui::BeginPopupContextItem())
				{
					if (ImGui::MenuItem("Delete Entity"))
					{
						EditorLayer::Get()->SetEntityNull();
						scene->DestroyEntity(entity);
					}

					ImGui::EndPopup();
				}

				if (expand)
				{
					// TODO: children
					ImGui::TreePop();
				}
			});

		ImGui::End();
	}
}