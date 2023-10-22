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

	links
	{
		"%{Lib.glfw3}",
		"%{Lib.glfw3_mt}",
		"%{Lib.glfw3dll}",

		"%{Lib.Vulkan}",

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

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

		defines 
		{
			"EK_DEBUG"
		}

		links
		{
			"%{Lib.shaderc_debug}",
			"%{Lib.SPIRV_Cross_debug}",
			"%{Lib.SPIRV_Cross_GLSL_debug}",
		}

	filter "configurations:Release"
		defines "EK_RELEASE"
		runtime "Release"
		optimize "On"

		links
		{
			"%{Lib.shaderc_release}",
			"%{Lib.SPIRV_Cross_release}",
			"%{Lib.SPIRV_Cross_GLSL_release}",
		}

	filter "configurations:Dist"
		defines "EK_DIST"
		runtime "Release"
		optimize "On"

		links
		{
			"%{Lib.shaderc_release}",
			"%{Lib.SPIRV_Cross_release}",
			"%{Lib.SPIRV_Cross_GLSL_release}",
		}