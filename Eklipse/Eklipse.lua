project "Eklipse"
	location "./"
	kind "SharedLib"
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
		"EK_ENABLE_ASSERTS",

		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE",
		"YAML_CPP_STATIC_DEFINE",
		"SPDLOG_COMPILED_LIB",
	}

	includedirs
	{
		"src",

		"%{Include.ScriptAPI}",

		"%{Include.Vulkan}",
		"%{Include.glad}",

        "%{Include.glslang}",
		"%{Include.shaderc}",
		"%{Include.SPIRV_Cross}",
        
		"%{Include.vk_mem_alloc}",
		"%{Include.glfw}",	
		"%{Include.spdlog}",
		"%{Include.glm}",
		"%{Include.imgui}",
		"%{Include.stb_image}",
		"%{Include.tiny_obj_loader}",
		"%{Include.entt}",
		"%{Include.imguizmo}",
		"%{Include.yamlcpp}",
        "%{Include.filewatch}",
		"%{Include.nfd}",
		"%{Include.dylib}"
	}

	links
	{
		"%{Lib.Vulkan}",

        "shaderc",
        "spirv_tools",
        "glslang",
        "glfw",
		"glad",
		"spdlog",
		"imgui",
		"imguizmo",
		"yamlcpp",
		"nfd"
	}

	postbuildcommands
	{
		"{COPYFILE} %{cfg.targetdir}/Eklipse.dll %{cfg.targetdir}/../EklipseEditor/"
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
			"%{Lib.SPIRV_Cross_debug}",
			"%{Lib.SPIRV_Cross_GLSL_debug}"
		}

	filter "configurations:Release"
		defines "EK_RELEASE"
		runtime "Release"
		optimize "On"

		links
		{
			"%{Lib.SPIRV_Cross_release}",
			"%{Lib.SPIRV_Cross_GLSL_release}"
		}

	filter "configurations:Dist"
		defines "EK_DIST"
		runtime "Release"
		optimize "On"

		links
		{
			"%{Lib.SPIRV_Cross_release}",
			"%{Lib.SPIRV_Cross_GLSL_release}",
		}