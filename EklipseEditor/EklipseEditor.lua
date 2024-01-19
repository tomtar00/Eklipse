project "EklipseEditor"
	location "./"
	kind "ConsoleApp" -- WindowedApp
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/obj/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	includedirs
	{
		"src",
		"%{wks.location}/Eklipse/src",

		"%{Include.ScriptAPI}",

		"%{Include.glfw}",
		"%{Include.glm}",
		"%{Include.imgui}",
		"%{Include.spdlog}",
		"%{Include.entt}",
		"%{Include.imguizmo}",
		"%{Include.nfd}",
		"%{Include.yamlcpp}",
		"%{Include.filewatch}",
		"%{Include.dylib}"
	}

	links
	{
		"Eklipse",
		"EklipseScriptAPI",

		"spdlog",
		"imgui",
		"imguizmo",
		"yamlcpp",
	}

	defines
	{
		"EK_ENABLE_ASSERTS",

        "_CRT_SECURE_NO_WARNINGS",
        "YAML_CPP_STATIC_DEFINE",
		"SPDLOG_COMPILED_LIB"
	}

	postbuildcommands
	{
		"{COPYDIR} %{prj.location}/Assets %{cfg.targetdir}/Assets",
		"{COPYDIR} %{prj.location}/Resources %{cfg.targetdir}/Resources",
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

	filter "configurations:Release"
		defines "EK_RELEASE"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines "EK_DIST"
		runtime "Release"
		optimize "On"