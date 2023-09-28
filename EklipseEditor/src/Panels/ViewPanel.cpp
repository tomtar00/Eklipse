#include "ViewPanel.h"
#include <Eklipse.h>

namespace Editor
{
	void ViewPanel::OnGUI(float deltaTime)
	{
		EK_PROFILE();

		ImGui::Begin("View");

		m_viewportPosition = ImGui::GetWindowPos();
		m_viewportSize = ImGui::GetContentRegionAvail();
		Eklipse::Application::Get().GUI->DrawViewport(m_viewportSize.x, m_viewportSize.y);
		m_aspectRatio = m_viewportSize.x / m_viewportSize.y;

		ImGui::End();
	}
}