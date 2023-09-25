project "Eklipse"
	location "./"
	kind "StaticLib"
	language "C++"
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
		"src",

		"%{Include.glfw}",	
		"%{Include.spdlog}",
		"%{Include.Vulkan}",
		"%{Include.glm}",
		"%{Include.ImGui}",
		"%{Include.Glad}",
		"%{Include.stb_image}",
		"%{Include.tiny_obj_loader}",
		"%{Include.vk_mem_alloc}",
		"%{Include.entt}"
	}
	libdirs
	{
		"%{Lib.glfw}",
		"%{Lib.Vulkan}"
	}

	links
	{
		"glfw3.lib",
		"glfw3_mt.lib",
		"glfw3dll.lib",

		"vulkan-1",

		"ImGui",
		"Glad"
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines
		{
			"EK_PLATFORM_WINDOWS",
			"EK_BUILD_DLL"
		}

		--postbuildcommands
		--{
		--	"{COPY} %{cfg.targetdir}\\Eklipse.dll %{cfg.targetdir}\\..\\EklipseEditor"
		--}

	filter "configurations:Debug"
		defines 
		{
			"EK_DEBUG",
			"EK_INCLUDE_DEBUG_LAYER"
		}
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