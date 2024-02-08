#pragma once
#include "Colors.h"
#include <imgui.h>
#include <glm/glm.hpp>
#include <misc/cpp/imgui_stdlib.h>
#include <Eklipse/Assets/Asset.h>

#define EK_PROPERTY_WIDTH 100.0f

namespace ImGui
{
    IMGUI_API bool InputAsset(const void* id, const char* label, Eklipse::AssetType assetType, Eklipse::AssetHandle& assetHandle);

    IMGUI_API bool InputFilePath(const void* id, const char* label, Eklipse::Path& path, const Eklipse::Vec<Eklipse::String>& requiredExtensions, const Eklipse::Path& defaultPath);
    IMGUI_API bool InputFilePath(const void* id, const char* label, Eklipse::Path& path, const Eklipse::Vec<Eklipse::String>& requiredExtensions);
    IMGUI_API bool InputDirPath(const void* id, const char* label, Eklipse::Path& path, const Eklipse::Path& defaultPath);
    IMGUI_API bool InputDirPath(const void* id, const char* label, Eklipse::Path& path);

    IMGUI_API void InputVec3(const void* id, const char* label, float labelWidth, glm::vec3& vec, float speed, float resetValue);

    template<typename UiFunction>
    void DrawProperty(const void* id, const char* label, UiFunction function, bool fullWidth = true)
    {
        ImGui::PushID(id);
        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, EK_PROPERTY_WIDTH);
        ImGui::Text(label);
        ImGui::NextColumn();

        if (fullWidth)
            ImGui::PushItemWidth(-FLT_MIN);

        function();

        if (fullWidth)
            ImGui::PopItemWidth();

        ImGui::Columns(1);
        ImGui::PopID();
    }
}