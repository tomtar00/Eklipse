#pragma once
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include <Eklipse/Assets/Asset.h>

namespace ImGui
{
    EK_API IMGUI_API bool InputAsset(const void* id, const char* label, Eklipse::AssetType assetType, Eklipse::AssetHandle& assetHandle);

    EK_API IMGUI_API bool InputPath(const void* id, const char* label, Eklipse::Path& path, const std::vector<Eklipse::String>& requiredExtensions);
    EK_API IMGUI_API bool InputDir(const void* id, const char* label, Eklipse::Path& path);
}