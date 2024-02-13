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

	filter "system:windows"
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
            "EK_DEBUG",
		    "EK_ENABLE_ASSERTS"
	    }
		links
		{
			"%{Lib.SPIRV_Cross_debug}",
			"%{Lib.SPIRV_Cross_GLSL_debug}"
		}
		postbuildcommands
        {
            "{COPYFILE} %{cfg.targetdir}/Eklipse.dll %{cfg.targetdir}/../EklipseEditor",
            "{COPYFILE} %{cfg.targetdir}/Eklipse.dll %{cfg.targetdir}/../EklipseRuntime",
		    "{COPYFILE} %{cfg.targetdir}/Eklipse.dll %{wks.location}/EklipseEditor/Resources/Export/Debug",
        }
        
	filter "configurations:Release"
		runtime "Release"
		optimize "On"

        defines
	    {
            "EK_RELEASE",
		    "EK_ENABLE_ASSERTS"
	    }
		links
		{
			"%{Lib.SPIRV_Cross_release}",
			"%{Lib.SPIRV_Cross_GLSL_release}"
		}
		postbuildcommands
        {
            "{COPYFILE} %{cfg.targetdir}/Eklipse.dll %{cfg.targetdir}/../EklipseEditor",
            "{COPYFILE} %{cfg.targetdir}/Eklipse.dll %{cfg.targetdir}/../EklipseRuntime",
		    "{COPYFILE} %{cfg.targetdir}/Eklipse.dll %{wks.location}/EklipseEditor/Resources/Export/Release",
        }

	filter "configurations:Dist"
		runtime "Release"
		optimize "On"

        defines 
        {
            "EK_DIST"
        }
		links
		{
			"%{Lib.SPIRV_Cross_release}",
			"%{Lib.SPIRV_Cross_GLSL_release}",
		}
		postbuildcommands
        {
            "{COPYFILE} %{cfg.targetdir}/Eklipse.dll %{cfg.targetdir}/../EklipseEditor",
            "{COPYFILE} %{cfg.targetdir}/Eklipse.dll %{cfg.targetdir}/../EklipseRuntime",
		    "{COPYFILE} %{cfg.targetdir}/Eklipse.dll %{wks.location}/EklipseEditor/Resources/Export/Dist",
        }