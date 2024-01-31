#include "SettingsPanel.h"
#include <EditorLayer.h>

namespace Eklipse
{
	bool SettingsPanel::OnGUI(float deltaTime)
	{
		EK_PROFILE();

		if (!GuiPanel::OnGUI(deltaTime)) return false;

		ImGui::Begin("Settings");

		ImGui::SeparatorText("Renderer");
		if (Project::GetActive())
		{
			if (ImGui::Checkbox("V-Sync", &Renderer::GetSettings().Vsync))
			{
				Renderer::OnVsyncChanged(Renderer::GetSettings().Vsync);
			}

			static const char* msaaSamples[]{ "Off","x2","x4","x8" };
			if (ImGui::Combo("MSAA", &Renderer::GetSettings().MsaaSamplesIndex, msaaSamples, IM_ARRAYSIZE(msaaSamples)))
			{
				Renderer::OnMultiSamplingChanged(Renderer::GetSettings().GetMsaaSamples());
			}

			if (!EditorLayer::Get().IsPlaying()) // TODO: fix (Play > Change API > Stop > CRASH)
			{
				static const char* APIs[]{ "Vulkan", "OpenGL" };
				static int api = (int)Renderer::GetAPI();
				if (ImGui::Combo("Render API", &api, APIs, IM_ARRAYSIZE(APIs)))
				{
					Application::Get().SetAPI((ApiType)(api));
				}
			}
		}

		ImGui::SeparatorText("Scripting");
#if EK_PLATFORM_WINDOWS
		ImGui::InputFilePath("msbuild", "MSBuild Path", EditorLayer::Get().GetSettings().ScriptManagerSettings.MsBuildPath, { ".exe" });
#endif
		ImGui::SeparatorText("Preferences");
		ImGui::InputText("Theme", &EditorLayer::Get().GetSettings().theme);
		ImGui::InputDirPath("Project", "Projects Path", EditorLayer::Get().GetSettings().projectsPath);

		ImGui::End();

		return true;
	}
}
