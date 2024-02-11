#include <precompiled.h>
#include "Theme.h"
#include "Colors.h"
#include <imgui.h>
#include <ImGuizmo.h>

namespace Eklipse
{
    String ThemeToString(Theme theme)
    {
        switch (theme)
        {
            case Theme::Light:      return "Light";
            case Theme::Dark:       return "Dark";
            case Theme::DarkGrey:   return "DarkGrey";
            case Theme::DarkBlue:   return "DarkBlue";
        }

        EK_CORE_WARN("Unknown theme enum: {0}", (int)theme);
        return "Unknown";
    }
    Theme StringToTheme(const String& theme)
    {
        if (theme == "Light")       return Theme::Light;
        if (theme == "Dark")        return Theme::Dark;
        if (theme == "DarkGrey")    return Theme::DarkGrey;
        if (theme == "DarkBlue")    return Theme::DarkBlue;

        EK_CORE_WARN("Unknown theme string: {0}", theme);
        return Theme::DarkBlue;
    }

    void SetTheme(Theme theme)
    {
        auto& style = ImGui::GetStyle();

        if (theme == Theme::Light)
        {
            ImGui::StyleColorsLight();
        }
        else if (theme == Theme::Dark)
        {
            ImGui::StyleColorsDark();
        }
        else if (theme == Theme::DarkGrey)
        {
            ImGui::StyleColorsDark();

            // Text
            style.Colors[ImGuiCol_Text] = ImVec4(0.75f, 0.78f, 0.81f, 1.0f);

            // Window
            style.Colors[ImGuiCol_WindowBg] = ImVec4(0.09f, 0.09f, 0.09f, 1.00f);

            // Headers
            style.Colors[ImGuiCol_Header] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
            style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
            style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);

            // Buttons
            style.Colors[ImGuiCol_Button] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
            style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
            style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);

            // Frame BG
            style.Colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
            style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
            style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);

            // Tabs
            style.Colors[ImGuiCol_Tab] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
            style.Colors[ImGuiCol_TabHovered] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
            style.Colors[ImGuiCol_TabActive] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
            style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
            style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);

            // Title
            style.Colors[ImGuiCol_TitleBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
            style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
            style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
        }
        else if (theme == Theme::DarkBlue)
        {
            style.WindowBorderSize = 0.0f;
            style.FramePadding = ImVec2(10, 4);
            style.ScrollbarRounding = 0.0f;
            style.ScrollbarSize = 10.0f;
            style.GrabMinSize = 10.0f;
            style.TabRounding = 0.0f;
            style.WindowMenuButtonPosition = ImGuiDir_None;
            style.SeparatorTextBorderSize = 1.0f;
            style.SeparatorTextAlign = ImVec2(0.5f, 0.5f);
            style.SeparatorTextPadding = ImVec2(20.0f, 0.0f);
            style.DockingSeparatorSize = 1.0f;

            ImGui::StyleColorsDark();

            // Colors
            style.Colors[ImGuiCol_Text] = ImVec4(0.75f, 0.78f, 0.81f, 1.0f);
            style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.29f, 0.35f, 0.42f, 1.00f);

            style.Colors[ImGuiCol_WindowBg] = ImVec4(0.09f, 0.11f, 0.13f, 1.0f);

            style.Colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.25f, 0.30f, 1.00f);
            style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.28f, 0.34f, 0.40f, 1.00f);
            style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.33f, 0.41f, 0.49f, 1.00f);

            style.Colors[ImGuiCol_Button] = ImVec4(0.20f, 0.25f, 0.30f, 1.00f);
            style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.34f, 0.40f, 1.00f);
            style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.33f, 0.41f, 0.49f, 1.00f);

            style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.17f, 0.20f, 1.00f);

            style.Colors[ImGuiCol_Header] = ImVec4(0.20f, 0.25f, 0.30f, 1.00f);
            style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.28f, 0.34f, 0.40f, 1.00f);
            style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.33f, 0.41f, 0.49f, 1.00f);

            style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.11f, 0.14f, 0.17f, 1.00f);
            style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.17f, 0.21f, 0.25f, 1.00f);
            style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.25f, 0.30f, 0.36f, 1.00f);
            style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.33f, 0.40f, 0.48f, 1.00f);

            style.Colors[ImGuiCol_CheckMark] = ImVec4(0.36f, 0.43f, 0.51f, 1.00f);

            style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.36f, 0.43f, 0.51f, 1.00f);
            style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.22f, 0.27f, 0.32f, 1.00f);

            style.Colors[ImGuiCol_Separator] = ImVec4(0.32f, 0.37f, 0.43f, 1.00f);

            style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.17f, 0.21f, 0.25f, 1.00f);
            style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.25f, 0.30f, 0.36f, 1.00f);
            style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.33f, 0.40f, 0.48f, 1.00f);

            style.Colors[ImGuiCol_DockingPreview] = ImVec4(0.33f, 0.41f, 0.49f, 1.00f);

            style.Colors[ImGuiCol_PlotLines] = ImVec4(0.47f, 0.54f, 0.63f, 1.00f);

            style.Colors[ImGuiCol_TitleBg] = ImVec4(0.20f, 0.25f, 0.30f, 1.00f);
            style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.20f, 0.25f, 0.30f, 1.00f);
            style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.20f, 0.25f, 0.30f, 1.00f);

            style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.22f, 0.27f, 0.32f, 1.00f);

            style.Colors[ImGuiCol_Tab] = ImVec4(0.20f, 0.25f, 0.30f, 1.00f);
            style.Colors[ImGuiCol_TabHovered] = ImVec4(0.28f, 0.34f, 0.40f, 1.00f);
            style.Colors[ImGuiCol_TabActive] = ImVec4(0.27f, 0.32f, 0.38f, 1.00f);
            style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.20f, 0.25f, 0.30f, 1.00f);
            style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.33f, 0.41f, 0.49f, 1.00f);
        }

        // ImGuizmo
        {
            auto& gizmoStyle = ImGuizmo::GetStyle();

            gizmoStyle.TranslationLineThickness = 4.0f;
            gizmoStyle.TranslationLineArrowSize = 8.0f;
            gizmoStyle.RotationLineThickness = 4.0f;
            gizmoStyle.RotationOuterLineThickness = 4.0f;
            gizmoStyle.ScaleLineThickness = 4.0f;
            gizmoStyle.ScaleLineCircleSize = 8.0f;
            gizmoStyle.HatchedAxisLineThickness = 8.0f;
            gizmoStyle.CenterCircleSize = 5.0f;

            gizmoStyle.Colors[ImGuizmo::DIRECTION_X] = EK_COLOR_RED;
            gizmoStyle.Colors[ImGuizmo::DIRECTION_Y] = EK_COLOR_GREEN;
            gizmoStyle.Colors[ImGuizmo::DIRECTION_Z] = EK_COLOR_BLUE;
            gizmoStyle.Colors[ImGuizmo::PLANE_X] = EK_COLOR_RED;
            gizmoStyle.Colors[ImGuizmo::PLANE_Y] = EK_COLOR_GREEN;
            gizmoStyle.Colors[ImGuizmo::PLANE_Z] = EK_COLOR_BLUE;

            gizmoStyle.Colors[ImGuizmo::SELECTION] = ImVec4(1.000f, 0.500f, 0.062f, 0.541f);
            gizmoStyle.Colors[ImGuizmo::INACTIVE] = ImVec4(0.600f, 0.600f, 0.600f, 0.600f);
            gizmoStyle.Colors[ImGuizmo::TRANSLATION_LINE] = ImVec4(0.666f, 0.666f, 0.666f, 0.666f);
            gizmoStyle.Colors[ImGuizmo::SCALE_LINE] = ImVec4(0.250f, 0.250f, 0.250f, 1.000f);
            gizmoStyle.Colors[ImGuizmo::ROTATION_USING_BORDER] = ImVec4(1.000f, 0.500f, 0.062f, 1.000f);
            gizmoStyle.Colors[ImGuizmo::ROTATION_USING_FILL] = ImVec4(1.000f, 0.500f, 0.062f, 0.500f);
            gizmoStyle.Colors[ImGuizmo::HATCHED_AXIS_LINES] = ImVec4(0.000f, 0.000f, 0.000f, 0.500f);
            gizmoStyle.Colors[ImGuizmo::TEXT] = ImVec4(1.000f, 1.000f, 1.000f, 1.000f);
            gizmoStyle.Colors[ImGuizmo::TEXT_SHADOW] = ImVec4(0.000f, 0.000f, 0.000f, 1.000f);
        }
    }
}