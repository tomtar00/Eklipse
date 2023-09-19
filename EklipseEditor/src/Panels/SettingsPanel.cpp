#include "SettingsPanel.h"
#include <Eklipse/Renderer/Settings.h>

namespace Editor
{
	void SettingsPanel::OnGUI()
	{
		EK_PROFILE();

		ImGui::Begin("Settings");

		static bool v;
		if (ImGui::Checkbox("V-Sync", &v))
		{
			Eklipse::RendererSettings::SetVsync(v);
		}
		
		static const char* items[]{ "Off","x2","x4","x8"};
		static int m;
		if (ImGui::Combo("MSAA", &m, items, IM_ARRAYSIZE(items)))
		{
			int samples = 0;
			if (m > 0)
				samples = pow(2, m);
			Eklipse::RendererSettings::SetMsaaSamples(samples);
		}

		ImGui::End();
	}
}
