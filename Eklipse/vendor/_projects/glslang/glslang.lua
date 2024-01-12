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
        "ENABLE_HLSL",
        "ENABLE_OPT"
    }

	files
	{
	    glslang .. "SPIRV/GLSL.ext.AMD.h",
        glslang .. "SPIRV/GLSL.ext.EXT.h",
        glslang .. "SPIRV/GLSL.ext.KHR.h",
        glslang .. "SPIRV/GLSL.ext.NV.h",
        glslang .. "SPIRV/GLSL.ext.ARM.h",
        glslang .. "SPIRV/GLSL.ext.QCOM.h",
        glslang .. "SPIRV/GLSL.std.450.h",
        glslang .. "SPIRV/GlslangToSpv.cpp",
        glslang .. "SPIRV/GlslangToSpv.h",
        glslang .. "SPIRV/InReadableOrder.cpp",
        glslang .. "SPIRV/Logger.cpp",
        glslang .. "SPIRV/Logger.h",
        glslang .. "SPIRV/NonSemanticDebugPrintf.h",
        glslang .. "SPIRV/NonSemanticShaderDebugInfo100.h",
        glslang .. "SPIRV/SPVRemapper.cpp",
        glslang .. "SPIRV/SPVRemapper.h",
        glslang .. "SPIRV/SpvBuilder.cpp",
        glslang .. "SPIRV/SpvBuilder.h",
        glslang .. "SPIRV/SpvPostProcess.cpp",
        glslang .. "SPIRV/SpvTools.h",
        glslang .. "SPIRV/bitutils.h",
        glslang .. "SPIRV/disassemble.cpp",
        glslang .. "SPIRV/disassemble.h",
        glslang .. "SPIRV/doc.cpp",
        glslang .. "SPIRV/doc.h",
        glslang .. "SPIRV/hex_float.h",
        glslang .. "SPIRV/spirv.hpp",
        glslang .. "SPIRV/spvIR.h",
        glslang .. "glslang/GenericCodeGen/CodeGen.cpp",
        glslang .. "glslang/GenericCodeGen/Link.cpp",
        glslang .. "glslang/Include/BaseTypes.h",
        glslang .. "glslang/Include/Common.h",
        glslang .. "glslang/Include/ConstantUnion.h",
        glslang .. "glslang/Include/InfoSink.h",
        glslang .. "glslang/Include/InitializeGlobals.h",
        glslang .. "glslang/Include/PoolAlloc.h",
        glslang .. "glslang/Include/ResourceLimits.h",
        glslang .. "glslang/Include/ShHandle.h",
        glslang .. "glslang/Include/SpirvIntrinsics.h",
        glslang .. "glslang/Include/Types.h",
        glslang .. "glslang/Include/arrays.h",
        glslang .. "glslang/Include/intermediate.h",
        glslang .. "glslang/MachineIndependent/Constant.cpp",
        glslang .. "glslang/MachineIndependent/InfoSink.cpp",
        glslang .. "glslang/MachineIndependent/Initialize.cpp",
        glslang .. "glslang/MachineIndependent/Initialize.h",
        glslang .. "glslang/MachineIndependent/IntermTraverse.cpp",
        glslang .. "glslang/MachineIndependent/Intermediate.cpp",
        glslang .. "glslang/MachineIndependent/LiveTraverser.h",
        glslang .. "glslang/MachineIndependent/ParseContextBase.cpp",
        glslang .. "glslang/MachineIndependent/ParseHelper.cpp",
        glslang .. "glslang/MachineIndependent/ParseHelper.h",
        glslang .. "glslang/MachineIndependent/PoolAlloc.cpp",
        glslang .. "glslang/MachineIndependent/RemoveTree.cpp",
        glslang .. "glslang/MachineIndependent/RemoveTree.h",
        glslang .. "glslang/MachineIndependent/Scan.cpp",
        glslang .. "glslang/MachineIndependent/Scan.h",
        glslang .. "glslang/MachineIndependent/ScanContext.h",
        glslang .. "glslang/MachineIndependent/ShaderLang.cpp",
        glslang .. "glslang/MachineIndependent/SpirvIntrinsics.cpp",
        glslang .. "glslang/MachineIndependent/SymbolTable.cpp",
        glslang .. "glslang/MachineIndependent/SymbolTable.h",
        glslang .. "glslang/MachineIndependent/Versions.cpp",
        glslang .. "glslang/MachineIndependent/Versions.h",
        glslang .. "glslang/MachineIndependent/attribute.cpp",
        glslang .. "glslang/MachineIndependent/attribute.h",
        glslang .. "glslang/MachineIndependent/gl_types.h",
        glslang .. "glslang/MachineIndependent/glslang_tab.cpp",
        glslang .. "glslang/MachineIndependent/glslang_tab.cpp.h",
        glslang .. "glslang/MachineIndependent/intermOut.cpp",
        glslang .. "glslang/MachineIndependent/iomapper.cpp",
        glslang .. "glslang/MachineIndependent/iomapper.h",
        glslang .. "glslang/MachineIndependent/limits.cpp",
        glslang .. "glslang/MachineIndependent/linkValidate.cpp",
        glslang .. "glslang/MachineIndependent/localintermediate.h",
        glslang .. "glslang/MachineIndependent/parseConst.cpp",
        glslang .. "glslang/MachineIndependent/parseVersions.h",
        glslang .. "glslang/MachineIndependent/preprocessor/Pp.cpp",
        glslang .. "glslang/MachineIndependent/preprocessor/PpAtom.cpp",
        glslang .. "glslang/MachineIndependent/preprocessor/PpContext.cpp",
        glslang .. "glslang/MachineIndependent/preprocessor/PpContext.h",
        glslang .. "glslang/MachineIndependent/preprocessor/PpScanner.cpp",
        glslang .. "glslang/MachineIndependent/preprocessor/PpTokens.cpp",
        glslang .. "glslang/MachineIndependent/preprocessor/PpTokens.h",
        glslang .. "glslang/MachineIndependent/propagateNoContraction.cpp",
        glslang .. "glslang/MachineIndependent/propagateNoContraction.h",
        glslang .. "glslang/MachineIndependent/reflection.cpp",
        glslang .. "glslang/MachineIndependent/reflection.h",
        glslang .. "glslang/OSDependent/osinclude.h",
        glslang .. "glslang/Public/ShaderLang.h",

        glslang .. "glslang/HLSL/hlslParseHelper.h",
        glslang .. "glslang/HLSL/hlslParseables.h",
        glslang .. "glslang/HLSL/hlslScanContext.h",
        glslang .. "glslang/HLSL/hlslTokens.h",

        glslang .. "glslang/HLSL/hlslAttributes.cpp",
        glslang .. "glslang/HLSL/hlslAttributes.h",
        glslang .. "glslang/HLSL/hlslGrammar.cpp",
        glslang .. "glslang/HLSL/hlslGrammar.h",
        glslang .. "glslang/HLSL/hlslOpMap.cpp",
        glslang .. "glslang/HLSL/hlslOpMap.h",
        glslang .. "glslang/HLSL/hlslParseHelper.cpp",
        glslang .. "glslang/HLSL/hlslParseables.cpp",
        glslang .. "glslang/HLSL/hlslScanContext.cpp",
        glslang .. "glslang/HLSL/hlslTokenStream.cpp",
        glslang .. "glslang/HLSL/hlslTokenStream.h",

        glslang .. "SPIRV/SpvTools.cpp",

        glslang ..  "glslang/ResourceLimits/ResourceLimits.cpp",
        glslang ..  "glslang/Public/ResourceLimits.h",
        glslang ..  "glslang/Include/ResourceLimits.h",

        glslang ..  "StandAlone/DirStackFileIncluder.h",
        glslang ..  "StandAlone/StandAlone.cpp",
        -- glslang ..  "StandAlone/spirv-remap.cpp"
	}

	includedirs
	{
		glslang,
        "%{Include.spirv_tools}"
	}

	filter "system:windows"
		systemversion "latest"

        files
        {
            glslang .. "glslang/OSDependent/Windows/ossource.cpp"
        }

        defines
        {
            "GLSLANG_OSINCLUDE_WIN32"
        }

        -- prebuildcommands
        -- {
        --     "cd " .. glslang .. "&& python3 build_info.py ./ -i build_info.h.tmpl -o glslang/build_info.h",
        --     "cd " .. glslang .. "&& python3 gen_extension_headers.py -i ./glslang/ExtensionHeaders -o ./glslang/glsl_intrinsic_header.h"
        -- }

    filter "system:linux"

        files
        {
            glslang .. "glslang/OSDependent/Unix/ossource.cpp"
        }

        defines
        {
            "GLSLANG_OSINCLUDE_UNIX"
        }

        -- prebuildcommands
        -- {
        --     "cd " .. glslang .. "&& python3 build_info.py ./ -i build_info.h.tmpl -o glslang/build_info.h",
        --     "cd " .. glslang .. "&& python3 gen_extension_headers.py -i ./glslang/ExtensionHeaders -o ./glslang/glsl_intrinsic_header.h"
        -- }

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		runtime "Release"
		optimize "On"