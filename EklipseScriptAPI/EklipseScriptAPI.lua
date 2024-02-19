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
		"%{Include.filewatch}",
        "%{Include.shaderc}",
	}

	links
	{
		"Eklipse",

		"spdlog",
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
		runtime "Debug"
		symbols "On"

        defines
	    {
            "EK_DEBUG",
		    "EK_ENABLE_ASSERTS"
	    }
        postbuildcommands
        {
            "{COPYFILE} %{prj.location}/src/EklipseEngine.h %{wks.location}/EklipseEditor/Resources/Scripting/Include",
		    "{COPYDIR} %{prj.location}/src/ScriptAPI %{wks.location}/EklipseEditor/Resources/Scripting/Include/ScriptAPI",

		    "{COPYFILE} %{cfg.targetdir}/EklipseScriptAPI.dll %{cfg.targetdir}/../EklipseEditor/EklipseScriptAPI.dll",
            "{COPYFILE} %{cfg.targetdir}/EklipseScriptAPI.dll %{cfg.targetdir}/../EklipseRuntime/EklipseScriptAPI.dll",

            "{COPYFILE} %{cfg.targetdir}/EklipseScriptAPI.lib %{wks.location}/EklipseEditor/Resources/Export/Debug/EklipseScriptAPI.lib",
		    "{COPYFILE} %{cfg.targetdir}/EklipseScriptAPI.dll %{wks.location}/EklipseEditor/Resources/Export/Debug/EklipseScriptAPI.dll",
        }

	filter "configurations:Release"
		runtime "Release"
		optimize "On"

        defines
	    {
            "EK_RELEASE",
		    "EK_ENABLE_ASSERTS"
	    }
        postbuildcommands
        {
            "{COPYFILE} %{prj.location}/src/EklipseEngine.h %{wks.location}/EklipseEditor/Resources/Scripting/Include",
		    "{COPYDIR} %{prj.location}/src/ScriptAPI %{wks.location}/EklipseEditor/Resources/Scripting/Include/ScriptAPI",

		    "{COPYFILE} %{cfg.targetdir}/EklipseScriptAPI.dll %{cfg.targetdir}/../EklipseEditor/EklipseScriptAPI.dll",
            "{COPYFILE} %{cfg.targetdir}/EklipseScriptAPI.dll %{cfg.targetdir}/../EklipseRuntime/EklipseScriptAPI.dll",

            "{COPYFILE} %{cfg.targetdir}/EklipseScriptAPI.lib %{wks.location}/EklipseEditor/Resources/Export/Developement/EklipseScriptAPI.lib",
		    "{COPYFILE} %{cfg.targetdir}/EklipseScriptAPI.dll %{wks.location}/EklipseEditor/Resources/Export/Developement/EklipseScriptAPI.dll",
        }   

	filter "configurations:Dist"
		runtime "Release"
		optimize "On"

        defines 
        {
            "EK_DIST"
        }
        postbuildcommands
        {
            "{COPYFILE} %{prj.location}/src/EklipseEngine.h %{wks.location}/EklipseEditor/Resources/Scripting/Include",
		    "{COPYDIR} %{prj.location}/src/ScriptAPI %{wks.location}/EklipseEditor/Resources/Scripting/Include/ScriptAPI",

		    "{COPYFILE} %{cfg.targetdir}/EklipseScriptAPI.dll %{cfg.targetdir}/../EklipseEditor/EklipseScriptAPI.dll",
            "{COPYFILE} %{cfg.targetdir}/EklipseScriptAPI.dll %{cfg.targetdir}/../EklipseRuntime/EklipseScriptAPI.dll",

            "{COPYFILE} %{cfg.targetdir}/EklipseScriptAPI.lib %{wks.location}/EklipseEditor/Resources/Export/Release/EklipseScriptAPI.lib",
		    "{COPYFILE} %{cfg.targetdir}/EklipseScriptAPI.dll %{wks.location}/EklipseEditor/Resources/Export/Release/EklipseScriptAPI.dll",
        }