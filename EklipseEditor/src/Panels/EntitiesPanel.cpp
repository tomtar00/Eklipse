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
			if (ImGui::MenuItem("Empty Entity"))
				m_sceneContext->CreateEntity();
			if (ImGui::MenuItem("Camera"))
			{
                auto entity = m_sceneContext->CreateEntity("Camera");
                auto& cam = entity.AddComponent<CameraComponent>();
				cam.camera.m_isMain = true;
                EditorLayer::Get().SelectionInfo.type = SelectionType::ENTITY;
                EditorLayer::Get().SelectionInfo.entity = entity;
            }
			if (ImGui::MenuItem("Cube"))
			{
				auto entity = m_sceneContext->CreateEntity("Cube");
				auto& mesh = entity.AddComponent<MeshComponent>();
				mesh.materialHandle = EditorLayer::Get().GetDefault3DMaterialHandle();
				mesh.meshHandle = EditorLayer::Get().GetDefaultCubeHandle();
				mesh.material = AssetManager::GetAsset<Material>(mesh.materialHandle).get();
				mesh.mesh = AssetManager::GetAsset<Mesh>(mesh.meshHandle).get();
			}
			if (ImGui::MenuItem("Sphere"))
			{
                auto entity = m_sceneContext->CreateEntity("Sphere");
                auto& mesh = entity.AddComponent<MeshComponent>();
                mesh.materialHandle = EditorLayer::Get().GetDefault3DMaterialHandle();
                mesh.meshHandle = EditorLayer::Get().GetDefaultSphereHandle();
                mesh.material = AssetManager::GetAsset<Material>(mesh.materialHandle).get();
                mesh.mesh = AssetManager::GetAsset<Mesh>(mesh.meshHandle).get();
            }

			ImGui::EndPopup();
		}

		ImGui::Text("Scene: %s", m_sceneContext->GetName().c_str());
		EK_CORE_DBG("Scene: {}", m_sceneContext->GetName());
		ImGui::Separator();
		m_sceneContext->ForEachEntity([&](auto entityID)
		{
			Entity entity{ entityID, m_sceneContext };
			auto& name = entity.GetName();

			ImGuiTreeNodeFlags flags = (EditorLayer::Get().SelectionInfo.entity == entity ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow;
			bool expand = ImGui::TreeNodeEx((void*)(uint64_t)entityID, flags, name.c_str());
			
			if (ImGui::IsItemClicked())
			{
				DetailsSelectionInfo info{};
				info.type = SelectionType::ENTITY;
				info.entity = entity;
				EditorLayer::Get().SelectionInfo = info;
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
	void EntitiesPanel::SetContext(Scene* scene)
	{
		m_sceneContext = scene;
	}
}