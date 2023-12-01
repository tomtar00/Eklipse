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

    targetdir ("%{wks.location}/Build/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/Build/obj/" .. outputdir .. "/%{prj.name}")

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

    defines
    {
        "EK_BUILD_DLL"
    }

    extensions = {
        ["windows"] = ".dll",
        ["macos"] = ".dylib",
        ["linux"] = ".so"
    }
    
    for system, ext in pairs(extensions) do
        filter("system:" .. system)
            systemversion "latest"
            postbuildcommands
            {
                "{COPY} %{cfg.targetdir}/%{prj.name}" .. ext .. " ./Build"
            }
    end