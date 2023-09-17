#include "SettingsPanel.h"
#include <Eklipse/Renderer/Settings.h>

bool v;
int m;

namespace Editor
{
	void SettingsPanel::OnGUI()
	{
		ImGui::Begin("Settings");

		ImGui::Checkbox("V-Sync", &v);
		ImGui::InputInt("MSAA", &m);

		ImGui::End();
	}
}
