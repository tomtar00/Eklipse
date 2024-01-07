project "yamlcpp"
	location "./"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "Off"
    
    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/obj/" .. outputdir .. "/%{prj.name}")

    yamlcpp = vendordir .. "yamlcpp/"

    includedirs
    {
        yamlcpp .. "include"
    }

    defines 
    {
        "YAML_CPP_STATIC_DEFINE"
    }

    files
    {
        yamlcpp .. "include/**.h",
        yamlcpp .. "src/**.h",
        yamlcpp .. "src/**.cpp"
    }

    filter "toolset:gcc"
        buildoptions 
        {
            "-Wall", "-Wextra", "-Wshadow", "-Weffc++", "-Wno-long-long",
            "-pedantic", "-pedantic-errors"
        }
       
    filter "toolset:msc*"
        buildoptions 
        {
            "/W3", "/wd4127", "/wd4355"
        }
    
    filter "system:windows"
        systemversion "latest"
        defines 
        {
            "_CRT_SECURE_NO_WARNINGS"
		}

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        runtime "Release"
        optimize "On"

    filter "configurations:Dist"
		runtime "Release"
		optimize "On"