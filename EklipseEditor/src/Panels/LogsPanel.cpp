#include "LogsPanel.h"

namespace Editor
{
	void LogsPanel::OnGUI()
	{
		EK_PROFILE();

		ImGui::Begin("Logs");
		ImGui::Text("These are log messages");
		ImGui::End();
	}
}
