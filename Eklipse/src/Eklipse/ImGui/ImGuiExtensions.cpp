#include "precompiled.h"
#include "ImGuiExtensions.h"
#include <Eklipse/Utils/File.h>

bool ImGui::InputPath(const void* id, const char* label, Eklipse::Path& path, const std::vector<Eklipse::String>& requiredExtensions)
{
    ImGui::PushID(id);
    if (label)
    {
        ImGui::Text(label);
        ImGui::SameLine();
    }

    bool isCurrentlyValid = path.IsCurrentlyValid();
    if (!isCurrentlyValid)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
    }

    ImGui::PushItemWidth(-30);
    bool active = ImGui::InputTextWithHint("##InputPath", label ? "Start with '//' to browse assets" : nullptr, path.rdbuf());
    ImGui::PopItemWidth();

    if (!isCurrentlyValid)
    {
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
    }

    if (active)
    {
		path.ParseSelf();
        if (path.IsValid(requiredExtensions))
        {            
            path.SetCurrentlyValid(true);
        }
        else
        {
            path.SetCurrentlyValid(false);
            active = false;
        }
    }

    ImGui::SameLine();
    if (ImGui::Button("..."))
    {
        auto& result = Eklipse::OpenFileDialog(requiredExtensions);
        if (result.type == Eklipse::FileDialogResultType::SUCCESS)
        {
            path = result.path;
            active = true;
        }
	}
    
    ImGui::PopID();
    return active;
}

IMGUI_API bool ImGui::InputDir(const void* id, const char* label, Eklipse::Path& path)
{
    ImGui::PushID(id);
    if (label)
    {
        ImGui::Text(label);
        ImGui::SameLine();
    }

    bool isCurrentlyValid = path.IsCurrentlyValid();
    if (!isCurrentlyValid)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
    }

    ImGui::PushItemWidth(-30);
    bool active = ImGui::InputTextWithHint("##InputPath", "Select directory", path.rdbuf());
    ImGui::PopItemWidth();

    if (!isCurrentlyValid)
    {
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
    }

    if (active)
    {
        path.ParseSelf();
        if (path.IsValid())
        {
            path.SetCurrentlyValid(true);
        }
        else
        {
            path.SetCurrentlyValid(false);
        }
    }

    ImGui::SameLine();
    if (ImGui::Button("..."))
    {
        auto& result = Eklipse::OpenDirDialog();
        if (result.type == Eklipse::FileDialogResultType::SUCCESS)
            path = result.path;
    }

    ImGui::PopID();
    return active;
}