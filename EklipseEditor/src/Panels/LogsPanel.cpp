#include "LogsPanel.h"

namespace Editor
{
	void LogsPanel::OnGUI(float deltaTime)
	{
		EK_PROFILE();

		ImGui::Begin("Logs");
		ImGui::Text("These are log messages");
		ImGui::End();
	}
}
