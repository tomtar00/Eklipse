include "dependencies.lua"

workspace "Eklipse"
	architecture "x64"
	startproject "EklipseEditor"
	location "./"

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
vendordir = "%{wks.location}/Eklipse/vendor/"

include "Eklipse/Eklipse.lua"
include "EklipseEditor/EklipseEditor.lua"
include "EklipseScriptAPI/EklipseScriptAPI.lua"
include "EklipseRuntime/EklipseRuntime.lua"

include "Eklipse/vendor/_projects/glfw/glfw.lua"
include "Eklipse/vendor/_projects/glad/glad.lua"
include "Eklipse/vendor/_projects/imgui/imgui.lua"
include "Eklipse/vendor/_projects/imguizmo/imguizmo.lua"
include "Eklipse/vendor/_projects/nfd/nfd.lua"
include "Eklipse/vendor/_projects/spdlog/spdlog.lua"
include "Eklipse/vendor/_projects/yamlcpp/yamlcpp.lua"
include "Eklipse/vendor/_projects/glslang/glslang.lua"
include "Eklipse/vendor/_projects/spirv_tools/spirv_tools.lua"