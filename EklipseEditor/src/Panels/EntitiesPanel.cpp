#include "EntitiesPanel.h"
#include "../EditorLayer.h"

#include <Eklipse/Scene/Components.h>

namespace Editor
{
	void EntitiesPanel::OnGUI(float deltaTime)
	{
		EK_PROFILE();

		ImGui::Begin("Entities");
		auto& scene = Eklipse::Application::Get().GetScene();
		EK_ASSERT(scene != nullptr, "Scene is null!");

		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::MenuItem("New Entity"))
				scene->CreateEntity();

			ImGui::EndPopup();
		}

		scene->ForEachEntity([&](auto entityID)
		{
			Eklipse::Entity entity{ entityID, scene.get() };
			auto& nameComponent = entity.GetComponent<Eklipse::NameComponent>();

			bool expand = ImGui::TreeNodeEx(nameComponent.name.c_str(), ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth);
			
			if (ImGui::IsItemClicked())
			{
				DetailsSelectionInfo info{};
				info.type = SelectionType::Entity;
				info.entity = entity;
				EditorLayer::Get().SetSelection(info);

				EditorLayer::Get().GetDetailsPanel().Setup(nameComponent.name);
			}

			if (ImGui::BeginPopupContextItem())
			{
				if (ImGui::MenuItem("Delete Entity"))
				{
					EditorLayer::Get().ClearSelection();
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