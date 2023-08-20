#include "ViewPanel.h"
#include <Eklipse.h>

namespace EklipseEditor
{
	void ViewPanel::OnGUI()
	{
		ImGui::Begin("View");

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		Eklipse::Application::Get().m_guiLayer->GetImage(viewportPanelSize.x, viewportPanelSize.y);

		ImGui::End();
	}
}