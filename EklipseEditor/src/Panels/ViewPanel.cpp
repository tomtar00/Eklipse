#include "ViewPanel.h"
#include "EditorLayer.h"
#include <Eklipse.h>
#include <Eklipse/Scene/Components.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Eklipse
{
	bool ViewPanel::OnGUI(float deltaTime)
	{
		EK_PROFILE();

		if (!GuiPanel::OnGUI(deltaTime)) return false;

		if (EditorLayer::Get().GetEditorState() == EditorState::EDIT)
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		else
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 1, 1 });
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1);
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4{ 0.0f, 1.0f, 0, 1.0f });
		}
		ImGui::Begin("View");
		auto& camera = EditorLayer::Get().GetEditorCamera();
		auto* viewMatrix = glm::value_ptr(camera.GetViewMatrix());
		auto* projMatrix = glm::value_ptr(camera.GetProjectionMatrix());

		bool viewportHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows | ImGuiHoveredFlags_DockHierarchy);
		EditorLayer::Get().SetCanControlEditorCamera(viewportHovered);

		// Scene
		m_viewportPosition = ImGui::GetWindowPos();
		m_viewportSize = ImGui::GetContentRegionAvail();
		EditorLayer::Get().GUI->DrawViewport(EditorLayer::Get().GetViewportFramebuffer(), m_viewportSize.x, m_viewportSize.y);
		m_aspectRatio = m_viewportSize.x / m_viewportSize.y;

		if (EditorLayer::Get().GetEditorState() == EditorState::EDIT)
		{
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(m_viewportPosition.x, m_viewportPosition.y, m_viewportSize.x, m_viewportSize.y);
			ImGuizmo::DrawGrid(viewMatrix, projMatrix, glm::value_ptr(glm::mat4(1.0f)), 10.0f);

			ImGui::SetCursorPos({ 0, 30.0f });
			ImGui::Indent(5.0f);
			static int operation = 0;
			if (ImGui::RadioButton("Translate [Q]", &operation, 0) || (Input::IsKeyDown(KeyCode::Q) && viewportHovered))
			{
				m_gizmoOperation = ImGuizmo::TRANSLATE;
				operation = 0;
			}
			if (ImGui::RadioButton("Rotate [W]", &operation, 1) || (Input::IsKeyDown(KeyCode::W) && viewportHovered))
			{
				m_gizmoOperation = ImGuizmo::ROTATE;
				operation = 1;
			}
			if (ImGui::RadioButton("Scale [E]", &operation, 2) || (Input::IsKeyDown(KeyCode::E) && viewportHovered))
			{
				m_gizmoOperation = ImGuizmo::SCALE;
				operation = 2;
			}
			ImGui::Spacing();
			static int mode = 0;
			if (ImGui::RadioButton("World [A]", &mode, 0) || (Input::IsKeyDown(KeyCode::A) && viewportHovered))
			{
				m_gizmoMode = ImGuizmo::WORLD;
				mode = 0;
			}
			if (ImGui::RadioButton("Local [S]", &mode, 1) || (Input::IsKeyDown(KeyCode::S) && viewportHovered))
			{
				m_gizmoMode = ImGuizmo::LOCAL;
				mode = 1;
			}
			ImGui::Unindent(5.0f);

			// Gizmos
			if (EditorLayer::Get().SelectionInfo.type == SelectionType::ENTITY)
			{
				auto entity = EditorLayer::Get().SelectionInfo.entity;
				auto& transComp = entity.GetComponent<TransformComponent>();
			
				auto* pos = glm::value_ptr(transComp.transform.position);
				auto* rot = glm::value_ptr(transComp.transform.rotation);
				auto* scale = glm::value_ptr(transComp.transform.scale);
			
				auto* matrix = glm::value_ptr(transComp.transformMatrix);
			
				ImGuizmo::RecomposeMatrixFromComponents(pos, rot, scale, matrix);
				ImGuizmo::Manipulate(viewMatrix, projMatrix, m_gizmoOperation, m_gizmoMode, matrix, nullptr, nullptr);
				if (ImGuizmo::IsUsing())
					ImGuizmo::DecomposeMatrixToComponents(matrix, pos, rot, scale);
			}
		}

		ImGui::End();
		if (EditorLayer::Get().GetEditorState() == EditorState::EDIT)
			ImGui::PopStyleVar();
		else
		{
			ImGui::PopStyleVar(2);
			ImGui::PopStyleColor();
		}

		return true;
	}
}