#pragma once
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

namespace ImGui
{
	EK_API IMGUI_API bool InputPath(const void* id, const char* label, Eklipse::Path& path, const std::vector<std::string>& requiredExtensions, const std::function<void()>& callback);
	EK_API IMGUI_API bool InputDir(const void* id, const char* label, Eklipse::Path& path);
}