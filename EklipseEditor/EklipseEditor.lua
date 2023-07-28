project "EklipseEditor"
	location "../EklipseEditor"
	kind "ConsoleApp" -- WindowedApp
	language "C++"
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
		"src",
		"../Eklipse/src",

		"%{Include.glfw}",
		"%{Include.spdlog}",
		"%{Include.Vulkan}",
		"%{Include.ImGui}"
	}
	libdirs
	{
		"%{Lib.glfw}",
		"%{Lib.Vulkan}"
	}

	links
	{
		"Eklipse",
		"ImGui"
	}

	filter "system:windows"
		cppdialect "C++17"
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