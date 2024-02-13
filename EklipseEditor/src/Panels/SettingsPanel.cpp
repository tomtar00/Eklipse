#include "SettingsPanel.h"
#include <EditorLayer.h>

namespace Eklipse
{
	bool SettingsPanel::OnGUI(float deltaTime)
	{
		EK_PROFILE();

		if (!GuiPanel::OnGUI(deltaTime)) return false;

		ImGui::Begin("Settings");

		if (ImGui::CollapsingHeader("Editor Settings"))
		{
			ImGui::SeparatorText("Scripting");
#if EK_PLATFORM_WINDOWS
			ImGui::InputFilePath("msbuild", "MSBuild Path", EditorLayer::Get().GetSettings().ScriptManagerSettings.MsBuildPath, { ".exe" });
#endif

			ImGui::SeparatorText("Preferences");
			ImGui::DrawProperty("theme", "Theme", [&]() {
				static const char* themes[]{ "Light","Dark","DarkGrey","DarkBlue" };
				int theme = (int)EditorLayer::Get().GetSettings().theme;
				if (ImGui::Combo("##MSAA", &theme, themes, IM_ARRAYSIZE(themes)))
				{
					EditorLayer::Get().GetSettings().theme = (Theme)theme;
					SetTheme(EditorLayer::Get().GetSettings().theme);
				}
            });
			ImGui::InputDirPath("Project", "Projects Path", EditorLayer::Get().GetSettings().projectsPath);
			ImGui::DrawProperty("camera_rotation_speed", "Camera Rotation Speed", [&]() {
                ImGui::InputFloat("##CameraRotationSpeed", &EditorLayer::Get().GetSettings().editorCameraRotationSpeed);
            });
			ImGui::DrawProperty("camera_drag_speed", "Camera Drag Speed", [&]() {
                ImGui::InputFloat("##CameraDragSpeed", &EditorLayer::Get().GetSettings().editorCameraDragSpeed);
            });
		}

		if (Project::GetActive())
		{
			if (ImGui::CollapsingHeader("Project Settings"))
			{
				ImGui::SeparatorText("Renderer");

				ImGui::DrawProperty("vsync", "V-Sync", [&]() {
					if (ImGui::Checkbox("##V-Sync", &Renderer::GetSettings().Vsync))
					{
						Renderer::OnVsyncChanged(Renderer::GetSettings().Vsync);
					}
				});

				ImGui::DrawProperty("msaa", "MSAA", [&]() {
					static const char* msaaSamples[]{ "Off","x2","x4","x8" };
					if (ImGui::Combo("##MSAA", &Renderer::GetSettings().MsaaSamplesIndex, msaaSamples, IM_ARRAYSIZE(msaaSamples)))
					{
						Renderer::OnMultiSamplingChanged(Renderer::GetSettings().GetMsaaSamples());
					}
				});

				if (EditorLayer::Get().GetEditorState() == EditorState::EDIT) // TODO: fix (Play > Change API > Stop > CRASH)
				{
					ImGui::DrawProperty("graphics_api", "Graphics API", [&]() {
						static const char* APIs[]{ "Vulkan", "OpenGL" };
						static int api = (int)Renderer::GetAPI();
						if (ImGui::Combo("##API", &api, APIs, IM_ARRAYSIZE(APIs)))
						{
							Application::Get().SetAPI((ApiType)(api));
						}
					});
				}
			}
		}

		ImGui::End();

		return true;
	}
}
