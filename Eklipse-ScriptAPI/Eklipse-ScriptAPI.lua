project "Eklipse-ScriptAPI"
	location "./"
	kind "SharedLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
	objdir ("../obj/" .. outputdir .. "/%{prj.name}")

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
		"%{Include.ImGui}",
		"%{Include.spdlog}",
		"%{Include.entt}",
		"%{Include.ImGuizmo}",
		"%{Include.nativefiledialog}",
		"%{Include.yaml_cpp}",
		"%{Include.filewatch}"
	}

	links
	{
		"Eklipse"
	}

    postbuildcommands
	{
		"{COPYFILE} %{prj.location}/src/EklipseEngine.h %{wks.location}/EklipseEditor/Resources/Scripting/Include",
		"{COPYFILE} %{prj.location}/src/Internal.h %{wks.location}/EklipseEditor/Resources/Scripting/Include",

        "{COPYFILE} %{cfg.targetdir}/Eklipse-ScriptAPI.lib %{wks.location}/EklipseEditor/Resources/Scripting/Lib",
        "{COPYFILE} %{cfg.targetdir}/Eklipse-ScriptAPI.dll ../bin/" .. outputdir .. "/EklipseEditor"
    }

    defines
    {
        "EK_BUILD_DLL"
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