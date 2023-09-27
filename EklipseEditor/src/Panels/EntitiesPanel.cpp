#include "EntitiesPanel.h"
#include "../EditorLayer.h"

#include <Eklipse/Scene/Components.h>

namespace Editor
{
	void EntitiesPanel::OnGUI(float deltaTime)
	{
		EK_PROFILE();

		ImGui::Begin("Entities");
		static Eklipse::Scene* scene = Eklipse::Application::Get().GetScene();

		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::MenuItem("New Entity"))
				scene->CreateEntity();
			if (ImGui::MenuItem("New Test Mesh")) // TODO: remove this later
				scene->CreateTestMesh("Test Mesh");

			ImGui::EndPopup();
		}

		scene->All([&](auto entityID)
			{
				Eklipse::Entity entity = { entityID, scene };
				auto& nameComponent = entity.GetComponent<Eklipse::NameComponent>();

				bool expand = ImGui::TreeNodeEx(nameComponent.name.c_str(), ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth);
				
				if (ImGui::IsItemClicked())
				{
					EditorLayer::Get()->GetDetailsPanel().Setup(entity, nameComponent.name);
				}

				if (ImGui::BeginPopupContextItem())
				{
					if (ImGui::MenuItem("Delete Entity"))
					{
						EditorLayer::Get()->GetDetailsPanel().SetEntityDeleted(true);
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