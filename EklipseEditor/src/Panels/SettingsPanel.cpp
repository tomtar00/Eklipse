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
					static bool vsync = Renderer::GetSettings().presentMode != PresentMode::IMMEDIATE;
					if (ImGui::Checkbox("##V-Sync", &vsync))
					{
						Renderer::OnPresentModeChanged(vsync ? PresentMode::FIFO : PresentMode::IMMEDIATE);
					}
				});

				ImGui::DrawProperty("msaa", "MSAA", [&]() {
					static const char* msaaSamples[]{ "Off","x2","x4","x8" };
					static int msaa = Renderer::GetSettings().MsaaSamplesIndex;
					if (ImGui::Combo("##MSAA", &msaa, msaaSamples, IM_ARRAYSIZE(msaaSamples)))
					{
						uint32_t samples = 1 << msaa;
						Framebuffer* framebuffer = EditorLayer::Get().GetViewportFramebuffer();
						Renderer::OnMultiSamplingChanged(framebuffer, samples);
					}
				});

				if (EditorLayer::Get().GetEditorState() == EditorState::EDIT) // TODO: fix (Play > Change API > Stop > CRASH)
				{
					ImGui::DrawProperty("graphics_api", "Graphics API", [&]() {
						static const char* APIs[]{ "Vulkan", "OpenGL" };
						static int api = (int)Renderer::GetGraphicsAPIType();
						if (ImGui::Combo("##API", &api, APIs, IM_ARRAYSIZE(APIs)))
						{
							Application::Get().SetGraphicsAPIType((GraphicsAPI::Type)(api));
						}
					});
				}

				ImGui::DrawProperty("pipeline_type", "Pipeline Type", [&]() {
					static const char* Types[]{ "Rasterization", "Ray Tracing" };
					static int type = (int)Renderer::GetPipelineType();
					if (ImGui::Combo("##Type", &type, Types, IM_ARRAYSIZE(Types)))
					{
						Renderer::RequestPipelineTypeChange((Pipeline::Type)(type));
					}
				});

				if (Renderer::GetPipelineType() == Pipeline::Type::RayTracing)
				{
					auto& rtContext = std::static_pointer_cast<RayTracingContext>(Renderer::GetRendererContext());

					ImGui::DrawProperty("accumulate", "Accumulate", [&]() {
						if (ImGui::Checkbox("##Accumulate", &Renderer::GetSettings().accumulate))
						{
							rtContext->SetAccumulate(Renderer::GetSettings().accumulate);
						}
					});
					ImGui::DrawProperty("rays_per_pixel", "Rays Per Pixel", [&]() {
						if (ImGui::InputInt("##RaysPerPixel", &Renderer::GetSettings().raysPerPixel))
						{
							rtContext->SetRaysPerPixel(Renderer::GetSettings().raysPerPixel);
						}
					});
					ImGui::DrawProperty("max_bounces", "Max Bounces", [&]() {
						if (ImGui::InputInt("##MaxBounces", &Renderer::GetSettings().maxBounces))
						{
							rtContext->SetMaxBounces(Renderer::GetSettings().maxBounces);
						}
					});

					ImGui::SeparatorText("Sky");
					ImGui::DrawProperty("sky_color_horizon", "Sky Color Horizon", [&]() {
						if (ImGui::ColorEdit3("##SkyColorHorizon", &Renderer::GetSettings().skyColorHorizon[0]))
						{
							rtContext->SetSkyColorHorizon(Renderer::GetSettings().skyColorHorizon);
						}
                    });
					ImGui::DrawProperty("sky_color_zenith", "Sky Color Zenith", [&]() {
						if (ImGui::ColorEdit3("##SkyColorZenith", &Renderer::GetSettings().skyColorZenith[0]))
						{
                            rtContext->SetSkyColorZenith(Renderer::GetSettings().skyColorZenith);
                        }
                    });
					ImGui::DrawProperty("ground_color", "Ground Color", [&]() {
						if (ImGui::ColorEdit3("##GroundColor", &Renderer::GetSettings().groundColor[0]))
						{
                            rtContext->SetGroundColor(Renderer::GetSettings().groundColor);
                        }
                    });
					ImGui::DrawProperty("sun_color", "Sun Color", [&]() {
						if (ImGui::ColorEdit3("##SunColor", &Renderer::GetSettings().sunColor[0]))
						{
                            rtContext->SetSunColor(Renderer::GetSettings().sunColor);
                        }
                    });
					ImGui::DrawProperty("sun_direction", "Sun Direction", [&]() {
						if (ImGui::DragFloat3("##SunDirection", &Renderer::GetSettings().sunDirection[0], 0.01f, -1.0f, 1.0f))
						{
                            rtContext->SetSunDirection(Renderer::GetSettings().sunDirection);
                        }
                    });
					ImGui::DrawProperty("sun_focus", "Sun Focus", [&]() {
						if (ImGui::DragFloat("##SunFocus", &Renderer::GetSettings().sunFocus))
						{
                            rtContext->SetSunFocus(Renderer::GetSettings().sunFocus);
                        }
                    });
					ImGui::DrawProperty("sun_intensity", "Sun Intensity", [&]() {
						if (ImGui::DragFloat("##SunIntensity", &Renderer::GetSettings().sunIntensity))
						{
							rtContext->SetSunIntensity(Renderer::GetSettings().sunIntensity);
						}
                    });
				}
			}
		}
		ImGui::End();
		return true;
	}
}
