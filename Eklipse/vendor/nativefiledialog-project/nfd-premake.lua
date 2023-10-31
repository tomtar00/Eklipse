project "NFD"
	location "./"
    kind "StaticLib"
    staticruntime "off"
    
    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/obj/" .. outputdir .. "/%{prj.name}")

    includedirs
    {
        "../nativefiledialog/src/include",
        "../nativefiledialog/src/*.h"
    }
    
    filter "system:windows"
        systemversion "latest"
        language "C++"
        cppdialect "C++17"

        files
        {
            "../nativefiledialog/src/include",
            "../nativefiledialog/src/nfd_common.h",
            "../nativefiledialog/src/*.cpp",
            "../nativefiledialog/src/nfd_common.c"
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