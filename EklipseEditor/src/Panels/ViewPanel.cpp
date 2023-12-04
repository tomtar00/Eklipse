#include "ViewPanel.h"
#include "EditorLayer.h"
#include <Eklipse.h>
#include <Eklipse/Scene/Components.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Editor
{
	bool ViewPanel::OnGUI(float deltaTime)
	{
		EK_PROFILE();

		if (!GuiPanel::OnGUI(deltaTime)) return false;

		ImGui::Begin("View");
		//auto entity = EditorLayer::Get()->GetSelection().entity; //EditorLayer::Get()->GetSelectedEntity();
		auto& camera = EditorLayer::Get().GetEditorCamera();
		auto* viewMatrix = glm::value_ptr(camera.GetViewMatrix());
		auto* projMatrix = glm::value_ptr(camera.GetProjectionMatrix());

		// Scene
		m_viewportPosition = ImGui::GetWindowPos();
		m_viewportSize = ImGui::GetContentRegionAvail();
		EditorLayer::Get().GUI->DrawViewport(m_viewportSize.x, m_viewportSize.y);
		m_aspectRatio = m_viewportSize.x / m_viewportSize.y;

		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(m_viewportPosition.x, m_viewportPosition.y, m_viewportSize.x, m_viewportSize.y);
		ImGuizmo::DrawGrid(viewMatrix, projMatrix, glm::value_ptr(glm::mat4(1.0f)), 10.0f);

		ImGui::SetCursorPos({ 0, 30.0f });
		ImGui::Indent(2.0f);
		static int operation = 0;
		if (ImGui::RadioButton("Translate [Q]", &operation, 0) || Eklipse::Input::IsKeyDown(Eklipse::KeyCode::Q))
		{
			m_gizmoOperation = ImGuizmo::TRANSLATE;
			operation = 0;
		}
		if (ImGui::RadioButton("Rotate [W]", &operation, 1) || Eklipse::Input::IsKeyDown(Eklipse::KeyCode::W))
		{
			m_gizmoOperation = ImGuizmo::ROTATE;
			operation = 1;
		}
		if (ImGui::RadioButton("Scale [E]", &operation, 2) || Eklipse::Input::IsKeyDown(Eklipse::KeyCode::E))
		{
			m_gizmoOperation = ImGuizmo::SCALE;
			operation = 2;
		}
		ImGui::Spacing();
		static int mode = 0;
		if (ImGui::RadioButton("World [A]", &mode, 0) || Eklipse::Input::IsKeyDown(Eklipse::KeyCode::A))
		{
			m_gizmoMode = ImGuizmo::WORLD;
			mode = 0;
		}
		if (ImGui::RadioButton("Local [S]", &mode, 1) || Eklipse::Input::IsKeyDown(Eklipse::KeyCode::S))
		{
			m_gizmoMode = ImGuizmo::LOCAL;
			mode = 1;
		}
		ImGui::Unindent(2.0f);

		// Gizmos
		if (EditorLayer::Get().GetSelection().type == SelectionType::Entity)
		{
			auto entity = EditorLayer::Get().GetSelection().entity;
			auto& transComp = entity.GetComponent<Eklipse::TransformComponent>();
		
			auto* pos = glm::value_ptr(transComp.transform.position);
			auto* rot = glm::value_ptr(transComp.transform.rotation);
			auto* scale = glm::value_ptr(transComp.transform.scale);
		
			auto* matrix = glm::value_ptr(transComp.transformMatrix);
		
			ImGuizmo::RecomposeMatrixFromComponents(pos, rot, scale, matrix);
			ImGuizmo::Manipulate(viewMatrix, projMatrix, m_gizmoOperation, m_gizmoMode, matrix, nullptr, nullptr);
			ImGuizmo::DecomposeMatrixToComponents(matrix, pos, rot, scale);
		}

		ImGui::End();

		return true;
	}
}