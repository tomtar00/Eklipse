#pragma once
#include <Eklipse.h>
#include <ImGuizmo.h>

namespace Eklipse
{
	class ViewPanel : public GuiPanel
	{
	public:
		virtual bool OnGUI(float deltaTime) override;
		float GetAspectRatio() const { return m_aspectRatio; }

		inline ImVec2 GetViewportSize() const { return m_viewportSize; }
		inline ImVec2 GetViewportPosition() const { return m_viewportPosition; }

	private:
		float m_aspectRatio{ 1.0f };
		ImVec2 m_viewportSize{ 0.0f, 0.0f };
		ImVec2 m_viewportPosition{ 0.0f, 0.0f };

		ImGuizmo::OPERATION m_gizmoOperation = ImGuizmo::TRANSLATE;
		ImGuizmo::MODE m_gizmoMode = ImGuizmo::WORLD;
	};
}