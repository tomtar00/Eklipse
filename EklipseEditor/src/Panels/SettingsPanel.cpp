#include "SettingsPanel.h"
#include <Eklipse/Renderer/Settings.h>

namespace Eklipse
{
	bool SettingsPanel::OnGUI(float deltaTime)
	{
		EK_PROFILE();

		if (!GuiPanel::OnGUI(deltaTime)) return false;

		ImGui::Begin("Settings");

		static bool v;
		if (ImGui::Checkbox("V-Sync", &v))
		{
			RendererSettings::SetVsync(v);
		}
		
		static const char* msaaSamples[]{ "Off","x2","x4","x8"};
		static int m = 0;
		if (ImGui::Combo("MSAA", &m, msaaSamples, IM_ARRAYSIZE(msaaSamples)))
		{
			int samples = 0;
			if (m > 0)
				samples = pow(2, m);
			RendererSettings::SetMsaaSamples(samples);
		}

		static const char* APIs[]{ "Vulkan", "OpenGL" };
		static int api = (int)Renderer::GetAPI() - 1;
		if (ImGui::Combo("Render API", &api, APIs, IM_ARRAYSIZE(APIs)))
		{
			Application::Get().SetAPI((ApiType)(api+1));
		}

		ImGui::End();

		return true;
	}
}
