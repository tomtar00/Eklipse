#include "EntitiesPanel.h"
#include "../EditorLayer.h"

#include <Eklipse/Scene/Components.h>

namespace Eklipse
{
	bool EntitiesPanel::OnGUI(float deltaTime)
	{
		EK_PROFILE();
		if (!GuiPanel::OnGUI(deltaTime)) return false;

		ImGui::Begin("Entities");
		if (!m_sceneContext)
		{
			EK_CORE_WARN("Scene context in entity panel is null!");
			ImGui::End();
			return true;
		}

		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::MenuItem("New Entity"))
				m_sceneContext->CreateEntity();

			ImGui::EndPopup();
		}

		ImGui::Text("Scene: %s", m_sceneContext->GetName().c_str());
		EK_CORE_DBG("Scene: {}", m_sceneContext->GetName());
		ImGui::Separator();
		m_sceneContext->ForEachEntity([&](auto entityID)
		{
			Entity entity{ entityID, m_sceneContext.get() };
			auto& nameComponent = entity.GetComponent<NameComponent>();

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
	void EntitiesPanel::SetContext(Ref<Scene> scene)
	{
		m_sceneContext = scene;
	}
}