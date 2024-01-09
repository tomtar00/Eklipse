project "glslang"
	location "./"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/obj/" .. outputdir .. "/%{prj.name}")

    glslang = vendordir .. "glslang/"

    defines
    {
        "ENABLE_HLSL"
    }

	files
	{
		glslang .. "glslang/GenericCodeGen/**.h",
        glslang .. "glslang/GenericCodeGen/**.hpp",
		glslang .. "glslang/GenericCodeGen/**.cpp",

        glslang .. "glslang/HLSL/**.h",
        glslang .. "glslang/HLSL/**.hpp",
		glslang .. "glslang/HLSL/**.cpp",

        glslang .. "glslang/Include/**.h",
        glslang .. "glslang/Include/**.hpp",
		glslang .. "glslang/Include/**.cpp",

        glslang .. "glslang/MachineIndependent/**.h",
        glslang .. "glslang/MachineIndependent/**.hpp",
		glslang .. "glslang/MachineIndependent/**.cpp",

        glslang .. "glslang/Public/**.h",
        glslang .. "glslang/Public/**.hpp",
		glslang .. "glslang/Public/**.cpp",

        glslang .. "glslang/ResourceLimits/**.h",
        glslang .. "glslang/ResourceLimits/**.hpp",
		glslang .. "glslang/ResourceLimits/**.cpp",

		glslang .. "SPIRV/*.h",
        glslang .. "SPIRV/*.hpp",
		glslang .. "SPIRV/*.cpp",
	}

	includedirs
	{
		glslang
	}

	filter "system:windows"
		systemversion "latest"

        files
        {
            glslang .. "glslang/OSDependent/Windows/**.h",
            glslang .. "glslang/OSDependent/Windows/**.hpp",
		    glslang .. "glslang/OSDependent/Windows/**.cpp",
        }

        prebuildcommands
        {
            "cd " .. glslang .. "&& python3 build_info.py ./ -i build_info.h.tmpl -o glslang/build_info.h"
        }

    filter "system:linux"

        files
        {
            glslang .. "glslang/OSDependent/Linux/**.h",
            glslang .. "glslang/OSDependent/Linux/**.hpp",
            glslang .. "glslang/OSDependent/Linux/**.cpp",
        }

        -- prebuildcommands

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		runtime "Release"
		optimize "On"