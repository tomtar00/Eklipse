#include "ViewPanel.h"
#include "EditorLayer.h"

#include <Eklipse.h>
#include <ImGuizmo.h>
#include <Eklipse/Scene/Components.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Editor
{
	void ViewPanel::OnGUI(float deltaTime)
	{
		EK_PROFILE();

		ImGui::Begin("View");
		auto entity = EditorLayer::Get()->GetSelectedEntity();

		// Scene
		m_viewportPosition = ImGui::GetWindowPos();
		m_viewportSize = ImGui::GetContentRegionAvail();
		Eklipse::Application::Get().GUI->DrawViewport(m_viewportSize.x, m_viewportSize.y);
		m_aspectRatio = m_viewportSize.x / m_viewportSize.y;

		// Gizmos
		if (!entity.IsNull())
		{
			auto& transComp = entity.GetComponent<Eklipse::TransformComponent>();
			ImGuizmo::SetDrawlist();

			auto* pos = glm::value_ptr(transComp.transform.position);
			auto* rot = glm::value_ptr(transComp.transform.rotation);
			auto* scale = glm::value_ptr(transComp.transform.scale);

			auto& camera = EditorLayer::Get()->GetEditorCamera();
			auto* matrix = glm::value_ptr(transComp.transformMatrix);

			ImGuizmo::RecomposeMatrixFromComponents(pos, rot, scale, matrix);
			auto& viewportPos = EditorLayer::Get()->GetViewPanel().GetViewportPosition();
			auto& viewportSize = EditorLayer::Get()->GetViewPanel().GetViewportSize();
			ImGuizmo::SetRect(viewportPos.x, viewportPos.y, viewportSize.x, viewportSize.y);

			auto* viewMatrix = glm::value_ptr(camera.GetViewMatrix());
			auto* projMatrix = glm::value_ptr(camera.GetProjectionMatrix());
			ImGuizmo::Manipulate(viewMatrix, projMatrix, ImGuizmo::TRANSLATE, ImGuizmo::WORLD, matrix, nullptr, nullptr);
			ImGuizmo::DecomposeMatrixToComponents(matrix, pos, rot, scale);
		}

		ImGui::End();
	}
}