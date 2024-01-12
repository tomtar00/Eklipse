project "spirv_tools"
	location "./"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/obj/" .. outputdir .. "/%{prj.name}")

    spirv_tools = vendordir .. "spirv_tools/"

	files
	{
		spirv_tools .. "source/*.h",
        spirv_tools .. "source/*.hpp",
		spirv_tools .. "source/*.cpp",

        spirv_tools .. "source/opt/*.h",
        spirv_tools .. "source/opt/*.hpp",
		spirv_tools .. "source/opt/*.cpp",

        spirv_tools .. "source/val/*.h",
        spirv_tools .. "source/val/*.hpp",
		spirv_tools .. "source/val/*.cpp",

        spirv_tools .. "source/util/*.h",
        spirv_tools .. "source/util/*.hpp",
		spirv_tools .. "source/util/*.cpp",
	}

	includedirs
	{
		spirv_tools,
        spirv_tools .. "include",

        "%{Include.spirv_headers}",
        "%{Include.spirv_headers}/spirv/unified1"
	}

    grammar_cmd_prefix = "cd " .. spirv_tools .. "utils && python3 generate_grammar_tables.py "
    registry_cmd_prefix = "cd " .. spirv_tools .. "utils && python3 generate_registry_tables.py "
    build_version_cmd_prefix = "cd " .. spirv_tools .. "utils && python3 update_build_version.py "

    function spirv_core_grammar(config_version)
        return "%{Include.spirv_headers}/spirv/" .. config_version .. "/spirv.core.grammar.json";
    end
    function extinst_debuginfo_grammar(config_version)
        return "%{Include.spirv_headers}/spirv/" .. config_version .."/extinst.debuginfo.grammar.json";
    end
    function extinst_cldebuginfo100_grammar(config_version)
        return "%{Include.spirv_headers}/spirv/" .. config_version .. "/extinst.opencl.debuginfo.100.grammar.json";
    end

    function get_gen_core_tables_cmd(config_version)
        core_insts_output = spirv_tools .. "core.insts-" .. config_version .. ".inc";
        operand_kinds_output = spirv_tools .. "operand.kinds-" .. config_version .. ".inc";

        opts = "";
        opts = opts .. " --spirv-core-grammar=" .. spirv_core_grammar(config_version);
        opts = opts .. " --extinst-debuginfo-grammar=" .. extinst_debuginfo_grammar(config_version);
        opts = opts .. " --extinst-cldebuginfo100-grammar=" .. extinst_cldebuginfo100_grammar(config_version);
        opts = opts .. " --core-insts-output=" .. core_insts_output;
        opts = opts .. " --operand-kinds-output=" .. operand_kinds_output;
        opts = opts .. " --output-language=c++"
        return grammar_cmd_prefix .. opts;
    end

    function get_gen_enum_string_mapping_cmd(config_version)
        extension_enum_output = spirv_tools .. "extension_enum.inc";
        enum_string_mapping_output = spirv_tools .. "enum_string_mapping.inc";

        opts = "";
        opts = opts .. " --spirv-core-grammar=" .. spirv_core_grammar(config_version);
        opts = opts .. " --extinst-debuginfo-grammar=" .. extinst_debuginfo_grammar(config_version);
        opts = opts .. " --extinst-cldebuginfo100-grammar=" .. extinst_cldebuginfo100_grammar(config_version);
        opts = opts .. " --extension-enum-output=" .. extension_enum_output;
        opts = opts .. " --enum-string-mapping-output=" .. enum_string_mapping_output;
        opts = opts .. " --output-language=c++"
        return grammar_cmd_prefix .. opts;
    end

    function get_gen_opencl_tables_cmd(config_version)
        extinst_opencl_grammar = "%{Include.spirv_headers}/spirv/" .. config_version .. "/extinst.opencl.std.100.grammar.json";
        opencl_insts_output = "%{Include.spirv_headers}/opencl.std.insts.inc";

        opts = "";
        opts = opts .. " --extinst-opencl-grammar=" .. extinst_opencl_grammar;
        opts = opts .. " --opencl-insts-output=" .. opencl_insts_output;
        return grammar_cmd_prefix .. opts;
    end

    function get_gen_glsl_tables_cmd(config_version)
        extinst_glsl_grammar = "%{Include.spirv_headers}/spirv/" .. config_version .. "/extinst.glsl.std.450.grammar.json";
        glsl_insts_output = "%{Include.spirv_headers}/glsl.std.450.insts.inc";

        opts = "";
        opts = opts .. " --extinst-glsl-grammar=" .. extinst_glsl_grammar;
        opts = opts .. " --glsl-insts-output=" .. glsl_insts_output;
        opts = opts .. " --output-language=c++"
        return grammar_cmd_prefix .. opts;
    end

    function get_gen_vendor_table_cmd(vendor_table, operand_kind_prefix)
        extinst_vendor_grammar = "%{Include.spirv_headers}/spirv/unified1/extinst." .. vendor_table .. ".grammar.json";
        vendor_insts_output = spirv_tools .. "" .. vendor_table .. ".insts.inc";
        vendor_operand_kind_prefix = operand_kind_prefix;

        opts = "";
        opts = opts .. " --extinst-vendor-grammar=" .. extinst_vendor_grammar;
        opts = opts .. " --vendor-insts-output=" .. vendor_insts_output;
        opts = opts .. " --vendor-operand-kind-prefix=" .. vendor_operand_kind_prefix;
        return grammar_cmd_prefix .. opts;
    end

    function get_gen_registry_file_cmd()
        xml = "%{Include.spirv_headers}/spirv/spir-v.xml";
        generator_output = spirv_tools .. "generators.inc"

        opts = "";
        opts = opts .. " --xml=" .. xml;
        opts = opts .. " --generator-output=" .. generator_output;
        return registry_cmd_prefix .. opts;
    end

    function get_gen_build_version_cmd()
        return build_version_cmd_prefix .. "" .. spirv_tools .. "CHANGES " .. spirv_tools .. "build-version.inc";
    end

    -- prebuildcommands
    -- {
    --     get_gen_core_tables_cmd("unified1"),
    --     get_gen_enum_string_mapping_cmd("unified1"),
    --     get_gen_opencl_tables_cmd("unified1"),
    --     get_gen_glsl_tables_cmd("unified1"),

    --     get_gen_vendor_table_cmd("debuginfo", ""),
    --     get_gen_vendor_table_cmd("opencl.debuginfo.100", "CLDEBUG100_"),
    --     get_gen_vendor_table_cmd("nonsemantic.shader.debuginfo.100", "SHDEBUG100_"),
    --     get_gen_vendor_table_cmd("spv-amd-gcn-shader", ""),
    --     get_gen_vendor_table_cmd("spv-amd-shader-ballot", ""),
    --     get_gen_vendor_table_cmd("spv-amd-shader-explicit-vertex-parameter", ""),
    --     get_gen_vendor_table_cmd("spv-amd-shader-trinary-minmax", ""),
    --     get_gen_vendor_table_cmd("nonsemantic.clspvreflection", ""),

    --     get_gen_registry_file_cmd(),
    --     get_gen_build_version_cmd()

    -- }

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