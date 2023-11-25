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
		"**.h",
		"**.cpp"
	}

	includedirs
	{
		"%{wks.location}/Eklipse/src",

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
        -- copy script api into source
		"{COPYFILE} EklipseEngine.h %{prj.location}../EklipseEditor/Resources/Scripting/Include",
        "{COPYFILE} %{cfg.targetdir}/Eklipse-ScriptAPI.lib %{prj.location}../EklipseEditor/Resources/Scripting/Lib"
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