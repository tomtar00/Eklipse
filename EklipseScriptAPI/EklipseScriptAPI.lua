project "EklipseScriptAPI"
	location "./"
	kind "SharedLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
	objdir ("../obj/" .. outputdir .. "/%{prj.name}")

	pchheader "precompiled.h"
	pchsource "src/precompiled.cpp"

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	includedirs
	{
		"%{wks.location}/Eklipse/src",
		"%{Include.ScriptAPI}",

		"%{Include.dylib}",
        "%{Include.glfw}",
		"%{Include.glm}",
		"%{Include.imgui}",
		"%{Include.spdlog}",
		"%{Include.entt}",
		"%{Include.imguizmo}",
		"%{Include.nfd}",
		"%{Include.yamlcpp}",
		"%{Include.filewatch}"
	}

	links
	{
		"Eklipse",

		"shaderc",
        "spirv_tools",
        "glslang",
        "glfw",
		"glad",
		"spdlog",
		"imgui",
		"imguizmo",
		"yamlcpp",
		"nfd"
	}

    postbuildcommands
	{
		"{COPYFILE} %{prj.location}/src/EklipseEngine.h %{wks.location}/EklipseEditor/Resources/Scripting/Include",
		"{COPYDIR} %{prj.location}/src/ScriptAPI %{wks.location}/EklipseEditor/Resources/Scripting/Include/ScriptAPI",
    }

    defines
    {
        "EK_SCRIPT_API_BUILD_DLL",

        "_CRT_SECURE_NO_WARNINGS",
		"SPDLOG_COMPILED_LIB"
    }

    filter "system:windows"
		systemversion "latest"

		defines
		{
			"EK_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "EK_DEBUG"
		runtime "Debug"
		symbols "On"

        postbuildcommands
        {
            "{COPYFILE} %{cfg.targetdir}/EklipseScriptAPI.lib %{wks.location}/EklipseEditor/Resources/Export/Debug",
		    "{COPYFILE} %{cfg.targetdir}/EklipseScriptAPI.dll %{wks.location}/EklipseEditor/Resources/Export/Debug",

			"{COPYFILE} %{cfg.targetdir}/EklipseScriptAPI.dll %{cfg.targetdir}/../EklipseEditor",
        }

	filter "configurations:Release"
		defines "EK_RELEASE"
		runtime "Release"
		optimize "On"

        postbuildcommands
        {
            "{COPYFILE} %{cfg.targetdir}/EklipseScriptAPI.lib %{wks.location}/EklipseEditor/Resources/Export/Release",
		    "{COPYFILE} %{cfg.targetdir}/EklipseScriptAPI.dll %{wks.location}/EklipseEditor/Resources/Export/Release",

			"{COPYFILE} %{cfg.targetdir}/EklipseScriptAPI.dll %{cfg.targetdir}/../EklipseEditor",
        }

	filter "configurations:Dist"
		defines "EK_DIST"
		runtime "Release"
		optimize "On"

        postbuildcommands
        {
            "{COPYFILE} %{cfg.targetdir}/EklipseScriptAPI.lib %{wks.location}/EklipseEditor/Resources/Export/Dist",
		    "{COPYFILE} %{cfg.targetdir}/EklipseScriptAPI.dll %{wks.location}/EklipseEditor/Resources/Export/Dist",

			"{COPYFILE} %{cfg.targetdir}/EklipseScriptAPI.dll %{cfg.targetdir}/../EklipseEditor",
        }