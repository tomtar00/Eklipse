include "../dependencies.lua"

workspace "Eklipse"
	architecture "x64"
	startproject "EklipseEditor"
	location "../"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}
	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.system}/%{cfg.buildcfg}-%{cfg.architecture}"

include "../Eklipse/Eklipse.lua"
include "../EklipseEditor/EklipseEditor.lua"

include "../Eklipse/vendor/ImGui-project/imgui-premake.lua"
include "../Eklipse/vendor/ImGuizmo-project/imguizmo-premake.lua"
include "../Eklipse/vendor/Glad/glad-premake.lua"
include "../Eklipse/vendor/nativefiledialog-project/nfd-premake.lua"
include "../Eklipse/vendor/yaml-cpp-project/yaml-cpp-premake.lua"