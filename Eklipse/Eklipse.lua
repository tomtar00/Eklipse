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
		"%{Include.entt}",
		"%{Include.ImGuizmo}",
		"%{Include.shaderc}",
		"%{Include.SPIRV_Cross}"
	}
	libdirs
	{
		"%{Lib.glfw}",
		"%{Lib.Vulkan}",
		"%{Lib.shaderc}",
		"%{Lib.SPIRV_Cross}"
	}

	links
	{
		"glfw3.lib",
		"glfw3_mt.lib",
		"glfw3dll.lib",

		"vulkan-1",
		"shaderc_shared",
		"spirv-cross-core",
		"spirv-cross-glsl",

		"ImGui",
		"ImGuizmo",
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
		runtime "Debug"
		symbols "On"

		defines 
		{
			"EK_DEBUG",
			"EK_INCLUDE_DEBUG_LAYER"
		}

	filter "configurations:Release"
		defines "EK_RELEASE"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines "EK_DIST"
		runtime "Release"
		optimize "On"