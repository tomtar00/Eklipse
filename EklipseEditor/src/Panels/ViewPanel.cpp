#include "ViewPanel.h"
#include <Eklipse.h>

namespace Editor
{
	void ViewPanel::OnGUI(float deltaTime)
	{
		EK_PROFILE();

		ImGui::Begin("View");

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		Eklipse::Application::Get().GUI->DrawViewport(viewportPanelSize.x, viewportPanelSize.y);
		m_aspectRatio = viewportPanelSize.x / viewportPanelSize.y;

		ImGui::End();
	}
}