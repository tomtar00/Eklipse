project "nfd"
	location "./"
    kind "StaticLib"
    staticruntime "off"
    
    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/obj/" .. outputdir .. "/%{prj.name}")

    nfd = vendordir .. "nfd/"

    includedirs
    {
        nfd .. "src/include",
        nfd .. "src/*.h"
    }
    
    filter "system:windows"
        systemversion "latest"
        language "C++"
        cppdialect "C++17"

        files
        {
            nfd .. "src/include",
            nfd .. "src/nfd_common.h",
            nfd .. "src/*.cpp",
            nfd .. "src/nfd_common.c"
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