#include "SettingsPanel.h"
#include <Eklipse/Renderer/Settings.h>

namespace Editor
{
	void SettingsPanel::OnGUI(float deltaTime)
	{
		EK_PROFILE();

		ImGui::Begin("Settings");

		static bool v;
		if (ImGui::Checkbox("V-Sync", &v))
		{
			Eklipse::RendererSettings::SetVsync(v);
		}
		
		static const char* msaaSamples[]{ "Off","x2","x4","x8"};
		static int m;
		if (ImGui::Combo("MSAA", &m, msaaSamples, IM_ARRAYSIZE(msaaSamples)))
		{
			int samples = 0;
			if (m > 0)
				samples = pow(2, m);
			Eklipse::RendererSettings::SetMsaaSamples(samples);
		}

		static const char* APIs[]{ "Vulkan", "OpenGL" };
		static int api;
		if (ImGui::Combo("Render API", &api, APIs, IM_ARRAYSIZE(APIs)))
		{
			Eklipse::Application::Get().SetAPI((Eklipse::ApiType)(api+1));
		}

		ImGui::End();
	}
}
