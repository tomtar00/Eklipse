workspace "__PRJ_NAME__-Scripts"
    architecture "x64"
    location "../../../"

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
    location "../../../"
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
        "EklipseScriptAPI"
    }
    defines
    {
        "_CRT_SECURE_NO_WARNINGS"
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
    end

    configs = { Debug="Debug", Developement="Release", Release="Dist" }
    for name, config in pairs(configs) do
        for system, ext in pairs(extensions) do
            filter { "system:" .. system, "configurations:" .. config }
                postbuildcommands
                {
                    "{COPY} %{cfg.targetdir}/%{prj.name}" .. ext .. " ./Scripts/Build/" .. name
                }
        end
    end