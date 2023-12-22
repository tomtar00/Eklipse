#include "EntitiesPanel.h"
#include "../EditorLayer.h"

#include <Eklipse/Scene/Components.h>

namespace Editor
{
	bool EntitiesPanel::OnGUI(float deltaTime)
	{
		EK_PROFILE();
		if (!GuiPanel::OnGUI(deltaTime)) return false;

		ImGui::Begin("Entities");
		if (!m_sceneContext)
			EK_CORE_WARN("Scene context in entity panel is null!");

		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::MenuItem("New Entity"))
				m_sceneContext->CreateEntity();

			ImGui::EndPopup();
		}

		m_sceneContext->ForEachEntity([&](auto entityID)
		{
			Eklipse::Entity entity{ entityID, m_sceneContext.get() };
			auto& nameComponent = entity.GetComponent<Eklipse::NameComponent>();

			bool expand = ImGui::TreeNodeEx(nameComponent.name.c_str(), ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth);
			
			if (ImGui::IsItemClicked())
			{
				DetailsSelectionInfo info{};
				info.type = SelectionType::ENTITY;
				info.entity = entity;
				EditorLayer::Get().SetSelection(info);

				EditorLayer::Get().GetDetailsPanel().Setup(nameComponent.name);
			}

			if (ImGui::BeginPopupContextItem())
			{
				if (ImGui::MenuItem("Delete Entity"))
				{
					EditorLayer::Get().ClearSelection();
					m_sceneContext->DestroyEntity(entity);
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
		return true;
	}
	void EntitiesPanel::SetContext(Eklipse::Ref<Eklipse::Scene> scene)
	{
		m_sceneContext = scene;
	}
}