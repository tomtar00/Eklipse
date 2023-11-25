workspace "__PRJ_NAME__-Scripts"
    architecture "x64"
    location "__PRJ_DIR__"

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

project "__PRJ_NAME__"
    location "__PRJ_DIR__"
    kind "SharedLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"

    targetdir ("../../bin/" .. outputdir .. "/%{prj.name}")
    objdir ("../../obj/" .. outputdir .. "/%{prj.name}")

    files
    {
        "Source/**.cpp",
        "Source/**.h"
    }

    includedirs
    {
        "__INCLUDE_DIR__"
    }
    libdirs
    {
        "__LIB_DIR__"
    }
    links
    {
        "Eklipse-ScriptAPI"
    }

    filter "system:windows"
		systemversion "latest"

        postbuildcommands
	    {
	    	"{COPY} %{cfg.targetdir}\\%{prj.name}.dll ./"
        }

    filter "system:macos"
		systemversion "latest"

        postbuildcommands
	    {
	    	"{COPY} %{cfg.targetdir}\\%{prj.name}.dylib ./"
        }

    filter "system:linux"
		systemversion "latest"

        postbuildcommands
	    {
	    	"{COPY} %{cfg.targetdir}\\%{prj.name}.so ./"
        }