project "shaderc"
	location "./"
    kind "StaticLib"
    staticruntime "off"
    
    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/obj/" .. outputdir .. "/%{prj.name}")

    shaderc = vendordir .. "shaderc/"

    defines
    {
        "ENABLE_HLSL",
        "SHADERC_IMPLEMENTATION"
    }

    files
    {
        shaderc .. "libshaderc_util/include/libshaderc_util/counting_includer.h",
        shaderc .. "libshaderc_util/include/libshaderc_util/exceptions.h",
        shaderc .. "libshaderc_util/include/libshaderc_util/file_finder.h",
        shaderc .. "libshaderc_util/include/libshaderc_util/format.h",
        shaderc .. "libshaderc_util/include/libshaderc_util/io_shaderc.h",
        shaderc .. "libshaderc_util/include/libshaderc_util/message.h",
        shaderc .. "libshaderc_util/include/libshaderc_util/mutex.h",
        shaderc .. "libshaderc_util/include/libshaderc_util/resources.h",
        shaderc .. "libshaderc_util/include/libshaderc_util/spirv_tools_wrapper.h",
        shaderc .. "libshaderc_util/include/libshaderc_util/string_piece.h",
        shaderc .. "libshaderc_util/include/libshaderc_util/universal_unistd.h",
        shaderc .. "libshaderc_util/include/libshaderc_util/version_profile.h",
        shaderc .. "libshaderc_util/src/compiler.cc",
        shaderc .. "libshaderc_util/src/file_finder.cc",
        shaderc .. "libshaderc_util/src/io_shaderc.cc",
        shaderc .. "libshaderc_util/src/message.cc",
        shaderc .. "libshaderc_util/src/resources.cc",
        shaderc .. "libshaderc_util/src/shader_stage.cc",
        shaderc .. "libshaderc_util/src/spirv_tools_wrapper.cc",
        shaderc .. "libshaderc_util/src/version_profile.cc",

        shaderc .. "libshaderc/include/shaderc/env.h",
        shaderc .. "libshaderc/include/shaderc/shaderc.h",
        shaderc .. "libshaderc/include/shaderc/shaderc.hpp",
        shaderc .. "libshaderc/include/shaderc/status.h",
        shaderc .. "libshaderc/include/shaderc/visibility.h",
        shaderc .. "libshaderc/src/shaderc.cc",
        shaderc .. "libshaderc/src/shaderc_private.h",
    }

    includedirs
    {
        shaderc .. "libshaderc/include",
        shaderc .. "libshaderc_util/include",
        "%{Include.glslang}",
        "%{Include.spirv_tools}",
        "%{Include.spirv_headers}"
    }
    
    filter "system:windows"
        systemversion "latest"
        language "C++"
        cppdialect "C++17"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        runtime "Release"
        optimize "On"

    filter "configurations:Dist"
		runtime "Release"
		optimize "On"