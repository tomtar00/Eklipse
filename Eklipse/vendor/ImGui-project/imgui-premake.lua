project "ImGui"
	location "./"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/obj/" .. outputdir .. "/%{prj.name}")

	files
	{
		"../imgui/*.h",
		"../imgui/*.cpp",

		"../imgui/misc/cpp/*.h",
		"../imgui/misc/cpp/*.cpp",

		"../imgui/backends/imgui_impl_glfw.h",
		"../imgui/backends/imgui_impl_glfw.cpp",

		"../imgui/backends/imgui_impl_vulkan.h",
		"../imgui/backends/imgui_impl_vulkan.cpp",

		"../imgui/backends/imgui_impl_opengl3.h",
		"../imgui/backends/imgui_impl_opengl3.cpp"
	}

	includedirs
	{
		"../imgui",
		"%{Include.glfw}",
		"%{Include.Vulkan}"
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