project "spdlog"
	location "./"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/obj/" .. outputdir .. "/%{prj.name}")

    spdlog = vendordir .. "spdlog/"

	files
	{
		spdlog .. "include/**.h",
		spdlog .. "src/**.cpp"
	}

	includedirs
	{
		spdlog .. "include"
	}

	defines
	{
		"SPDLOG_COMPILED_LIB"
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