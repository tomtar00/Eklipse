#include "ViewPanel.h"
#include <Eklipse.h>

namespace EklipseEditor
{
	void ViewPanel::OnGUI()
	{
		ImGui::Begin("View");

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		Eklipse::Application::Get().m_guiLayer->DrawViewport(viewportPanelSize.x, viewportPanelSize.y);

		ImGui::End();
	}
}