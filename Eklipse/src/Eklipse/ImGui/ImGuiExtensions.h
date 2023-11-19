#pragma once
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

namespace ImGui
{
	IMGUI_API bool InputPath(const char* id, const char* label, Eklipse::Path& path, const std::vector<std::string>& requiredExtensions, const std::function<void()>& callback);
}