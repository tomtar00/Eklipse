#include "ViewPanel.h"
#include <Eklipse.h>

namespace Editor
{
	void ViewPanel::OnGUI()
	{
		EK_PROFILE();

		ImGui::Begin("View");

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		Eklipse::Application::Get().m_guiLayer->DrawViewport(viewportPanelSize.x, viewportPanelSize.y);

		ImGui::End();
	}
}