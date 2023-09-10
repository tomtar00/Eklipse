#include "LogsPanel.h"

namespace Editor
{
	void LogsPanel::OnGUI()
	{
		ImGui::Begin("Logs");
		ImGui::Text("These are log messages");
		ImGui::End();
	}
}
