#pragma once
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include <Eklipse/Assets/Asset.h>

namespace ImGui
{
    IMGUI_API bool InputAsset(const void* id, const char* label, Eklipse::AssetType assetType, Eklipse::AssetHandle& assetHandle);

    IMGUI_API bool InputFilePath(const void* id, const char* label, Eklipse::Path& path, const Eklipse::Vec<Eklipse::String>& requiredExtensions);
    IMGUI_API bool InputDirPath(const void* id, const char* label, Eklipse::Path& path);
}