#include "DetailsPanel.h"
#include "EntitiesPanel.h"
#include "EditorLayer.h"

#include <misc/cpp/imgui_stdlib.h>
#include <glm/gtc/type_ptr.hpp>
#include <Eklipse/Scene/Components.h>

namespace Editor
{
	void DetailsPanel::Setup(std::string& name)
	{
		m_entityNameBuffer = name;
	}
	void DetailsPanel::OnGUI(float deltaTime)
	{
		EK_PROFILE();

		ImGui::Begin("Details");
		auto entity = EditorLayer::Get()->GetSelectedEntity();

		if (entity.IsNull())
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
					entity.GetComponent<Eklipse::NameComponent>().name = m_entityNameBuffer;
				}
			}
		}

		// Transform
		{
			auto& transComp = entity.GetComponent<Eklipse::TransformComponent>();

			ImGui::DragFloat3("Position", glm::value_ptr(transComp.transform.position), 0.1f);
			ImGui::DragFloat3("Rotation", glm::value_ptr(transComp.transform.rotation), 0.1f);
			ImGui::DragFloat3("Scale", glm::value_ptr(transComp.transform.scale), 0.1f);
		}

		// Camera
		{
			auto* cameraComp = entity.TryGetComponent<Eklipse::CameraComponent>();
			if (cameraComp)
			{
				ImGui::SliderFloat("FOV", &cameraComp->camera.m_fov, 0.0f, 180.0f);
				ImGui::SliderFloat("Near", &cameraComp->camera.m_nearPlane, 0.01f, 1000.0f);
				ImGui::SliderFloat("Far", &cameraComp->camera.m_farPlane, 0.01f, 1000.0f);
			}
		}

		// Mesh
		{
			auto* meshComp = entity.TryGetComponent<Eklipse::MeshComponent>();
			if (meshComp != nullptr)
				ImGui::Text("This is a mesh!");
		}

		ImGui::End();
	}
}
