#include "LogsPanel.h"

namespace EklipseEditor
{
	void LogsPanel::OnGUI()
	{
		ImGui::Begin("Logs");
		ImGui::Text("These are log messages");
		ImGui::End();
	}
}
