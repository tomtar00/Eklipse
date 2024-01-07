project "glad"
    location "./"
    kind "StaticLib"
    language "C"
    staticruntime "off"
    
    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/obj/" .. outputdir .. "/%{prj.name}")

    glad = vendordir .. "glad/"

    files
    {
        glad .. "include/glad/glad.h",
        glad .. "include/KHR/khrplatform.h",
        glad .. "src/glad.c"
    }

    includedirs
    {
        glad .. "include"
    }
    
    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        runtime "Release"
        optimize "On"

    filter "configurations:Dist"
		runtime "Release"
		optimize "On"