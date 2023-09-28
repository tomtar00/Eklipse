#include "DetailsPanel.h"
#include "EntitiesPanel.h"
#include "EditorLayer.h"

#include <misc/cpp/imgui_stdlib.h>
#include <glm/gtc/type_ptr.hpp>
#include <Eklipse/Scene/Components.h>
#include <ImGuizmo.h>

namespace Editor
{
	void DetailsPanel::Setup(Eklipse::Entity entity, std::string& name)
	{
		m_entity = entity;
		m_entityNameBuffer = name;
		m_entityDeleted = false;
	}
	void DetailsPanel::OnGUI(float deltaTime)
	{
		EK_PROFILE();

		ImGui::Begin("Details");
		if (m_entityDeleted || m_entity.isNull())
		{
			ImGui::End(); 
			return;
		}

		// Right-click menu
		{
			if (ImGui::BeginPopupContextWindow())
			{
				if (ImGui::MenuItem("Add Component 1"))
					EK_TRACE("Add Component 1");
				if (ImGui::MenuItem("Add Component 2"))
					EK_TRACE("Add Component 2");

				ImGui::EndPopup();
			}
		}

		// Name
		{
			if (!ImGui::InputText("Entity Name", &m_entityNameBuffer))
			{
				if (m_entityNameBuffer.size() > 0)
				{
					m_entity.GetComponent<Eklipse::NameComponent>().name = m_entityNameBuffer;
				}
			}
		}

		// Transform
		{
			ImGuizmo::SetDrawlist();

			auto& transComp = m_entity.GetComponent<Eklipse::TransformComponent>();

			// auto* pos = glm::value_ptr(transComp.transform.position);
			// auto* rot = glm::value_ptr(transComp.transform.rotation);
			// auto* scale = glm::value_ptr(transComp.transform.scale);

			auto& camera = EditorLayer::Get()->GetEditorCamera();
			auto* matrix = glm::value_ptr(transComp.GetTransformMatrix(camera.GetViewProjectionMatrix()));

			float matrixTranslation[3], matrixRotation[3], matrixScale[3];
			ImGuizmo::DecomposeMatrixToComponents(matrix, matrixTranslation, matrixRotation, matrixScale);
			ImGui::DragFloat3("Position", matrixTranslation, 0.1f);
			ImGui::DragFloat3("Rotation", matrixRotation, 0.1f);
			ImGui::DragFloat3("Scale", matrixScale, 0.1f);
			ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrix);

			/*auto& viewportPos = EditorLayer::Get()->GetViewPanel().GetViewportPosition();
			auto& viewportSize = EditorLayer::Get()->GetViewPanel().GetViewportSize();
			ImGuizmo::SetRect(viewportPos.x, viewportPos.y, viewportSize.x, viewportPos.y);*/
			ImGuiIO& io = ImGui::GetIO();
			ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
			ImGuizmo::Manipulate(glm::value_ptr(camera.GetViewMatrix()), glm::value_ptr(camera.GetProjectionMatrix()), ImGuizmo::TRANSLATE, ImGuizmo::WORLD, matrix, NULL, NULL);
		}

		// Camera
		{
			auto* cameraComp = m_entity.TryGetComponent<Eklipse::CameraComponent>();
			if (cameraComp)
			{
				ImGui::SliderFloat("FOV", &cameraComp->camera.m_fov, 0.0f, 180.0f);
				ImGui::SliderFloat("Near", &cameraComp->camera.m_nearPlane, 0.01f, 1000.0f);
				ImGui::SliderFloat("Far", &cameraComp->camera.m_farPlane, 0.01f, 1000.0f);
			}
		}

		// Mesh
		{
			auto* meshComp = m_entity.TryGetComponent<Eklipse::MeshComponent>();
			if (meshComp != nullptr)
				ImGui::Text("This is a mesh!");
		}

		ImGui::End();
	}
}
