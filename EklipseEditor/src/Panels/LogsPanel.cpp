#include "LogsPanel.h"

namespace Editor
{
	bool LogsPanel::OnGUI(float deltaTime)
	{
		EK_PROFILE();

		if (!GuiPanel::OnGUI(deltaTime)) return false;

		ImGui::Begin("Logs");
		ImGui::Text("These are log messages");
		ImGui::End();

		return true;
	}
}
