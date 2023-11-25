project "Eklipse"
	location "./"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
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

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE",
		"YAML_CPP_STATIC_DEFINE"
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
		"%{Include.SPIRV_Cross}",
		"%{Include.yaml_cpp}",
        "%{Include.filewatch}",
		"%{Include.nativefiledialog}"
	}

	links
	{
		"%{Lib.glfw3}",
		"%{Lib.glfw3_mt}",
		"%{Lib.glfw3dll}",
		"%{Lib.Vulkan}",

		"ImGui",
		"ImGuizmo",
		"Glad",
		"YAML",
		"NFD"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"EK_PLATFORM_WINDOWS",
			"EK_BUILD_DLL"
		}

	filter "configurations:Debug"
		defines "EK_DEBUG"
		runtime "Debug"
		symbols "On"

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