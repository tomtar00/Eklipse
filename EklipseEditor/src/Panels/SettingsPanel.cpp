#include "SettingsPanel.h"
#include <Eklipse/Renderer/Settings.h>

namespace Editor
{
	void SettingsPanel::OnGUI()
	{
		ImGui::Begin("Settings");

		static bool v;
		if (ImGui::Checkbox("V-Sync", &v))
		{
			Eklipse::RendererSettings::SetVsync(v);
		}
		
		static const char* items[]{ "Off","x2","x4" };
		static int m;
		if (ImGui::Combo("MSAA", &m, items, IM_ARRAYSIZE(items)))
		{
			Eklipse::RendererSettings::SetMsaaSamples(m);
		}

		ImGui::End();
	}
}
