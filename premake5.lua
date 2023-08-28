include "dependencies.lua"

workspace "Eklipse"
	architecture "x64"
	startproject "EklipseEditor"

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

include "Eklipse/Eklipse.lua"
include "EklipseEditor/EklipseEditor.lua"
include "Eklipse/vendor/imgui-premake.lua"
include "Eklipse/vendor/Glad/glad.lua"