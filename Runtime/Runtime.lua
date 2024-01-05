project "Runtime"
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
		"%{Include.ImGui}",
		"%{Include.spdlog}",
		"%{Include.entt}",
		"%{Include.ImGuizmo}",
		"%{Include.nativefiledialog}",
		"%{Include.yaml_cpp}",
		"%{Include.filewatch}",
		"%{Include.dylib}"
	}

	links
	{
		"Eklipse",
		"Eklipse-ScriptAPI"
	}

	defines
	{
		"EK_ENABLE_ASSERTS",
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
			"{COPYFILE} %{cfg.targetdir}/Runtime.exe %{wks.location}/EklipseEditor/Resources/Export/Debug"
		}

	filter "configurations:Release"
		defines "EK_RELEASE"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines "EK_DIST"
		runtime "Release"
		optimize "On"

		postbuildcommands
		{
			"{COPYFILE} %{cfg.targetdir}/Runtime.exe %{wks.location}/EklipseEditor/Resources/Export/Distribution"
		}