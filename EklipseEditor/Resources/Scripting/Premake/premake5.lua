workspace "__PRJ_NAME__-Scripts"
    architecture "x64"
    location "__PRJ_DIR__"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }
    flags
    {
        "MultiProcessorCompile"
    }

outputdir = "%{cfg.system}/%{cfg.buildcfg}-%{cfg.architecture}"

project "__PRJ_NAME__"
    location "__PRJ_DIR__"
    kind "SharedLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"
    symbols "Off"

    targetdir ("%{wks.location}/Scripts/Build/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/Scripts/Build/obj/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{wks.location}/Scripts/**.cpp",
        "%{wks.location}/Scripts/**.hpp",
        "%{wks.location}/Scripts/**.h",
    }

    includedirs
    {
        "%{wks.location}/Scripts",
        "__INCLUDE_DIR__"
    }
    libdirs
    {
        "__LIB_DIR__"
    }
    links
    {
        "Eklipse-ScriptAPI"
    }

    filter "configurations:Debug"
		defines "EK_DEBUG"
		runtime "Debug"

	filter "configurations:Release"
		defines "EK_RELEASE"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines "EK_DIST"
		runtime "Release"
		optimize "On"

    extensions = {
        ["windows"] = ".dll",
        ["macos"] = ".dylib",
        ["linux"] = ".so"
    }
    
    for system, ext in pairs(extensions) do
        filter("system:" .. system)
            systemversion "latest"
            defines
            {
                "EK_PLATFORM_" .. string.upper(system)
            }
            postbuildcommands
            {
                "{COPY} %{cfg.targetdir}/%{prj.name}" .. ext .. " ./Scripts/Build"
            }
    end