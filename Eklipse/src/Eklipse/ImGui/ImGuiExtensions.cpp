#include "precompiled.h"
#include "ImGuiExtensions.h"

bool ImGui::InputPath(const char* id, const char* label, Eklipse::Path& path, const std::vector<std::string>& requiredExtensions, const std::function<void()>& callback)
{
    ImGui::PushID(label);
    ImGui::Text(label);
    ImGui::SameLine();

    bool active = ImGui::InputText("##inputpath", path.rdbuf(), ImGuiInputTextFlags_CallbackEdit);

    if (active)
    {
		path.parseSelf();
        if (path.isValid(requiredExtensions))
        {
            EK_CORE_INFO("Valid path: {0} -- {1}", path.string(), path.full_string());
            callback();
        }
        else EK_CORE_WARN("Invalid path: {0} -- {1}", path.string(), path.full_string());
    }

    ImGui::PopID();

    return active;
}
