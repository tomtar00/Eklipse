project "imguizmo"
	location "./"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/obj/" .. outputdir .. "/%{prj.name}")

    imguizmo = vendordir .. "imguizmo/"

	files
	{
		imguizmo .. "*.h",
		imguizmo .. "*.cpp"
	}

	includedirs
	{
		imguizmo,
		"%{Include.glfw}",
		"%{Include.Vulkan}",
		"%{Include.imgui}"
	}
	libdirs
	{
		"%{Lib.glfw}",
		"%{Lib.Vulkan}"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		runtime "Release"
		optimize "On"