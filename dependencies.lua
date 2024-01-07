Include = {}
Lib = {}

-- Vulkan
Include["Vulkan"] = "%{wks.location}/Eklipse/vendor/Vulkan/include"
Lib["Vulkan"] = "%{wks.location}/Eklipse/vendor/Vulkan/lib/vulkan-1.lib"

-- ShaderC
Include["shaderc"] = "%{wks.location}/Eklipse/vendor/shaderc/include"
Lib["shaderc_debug"] = "%{wks.location}/Eklipse/vendor/shaderc/lib-debug/shaderc_sharedd.lib"
Lib["shaderc_release"] = "%{wks.location}/Eklipse/vendor/shaderc/lib-release/shaderc_shared.lib"

-- SPIRV-Cross
Include["SPIRV_Cross"] = "%{wks.location}/Eklipse/vendor/SPIRV_Cross/include"
Lib["SPIRV_Cross_debug"] = "%{wks.location}/Eklipse/vendor/SPIRV_Cross/lib-debug/spirv-cross-cored.lib"
Lib["SPIRV_Cross_GLSL_debug"] = "%{wks.location}/Eklipse/vendor/SPIRV_Cross/lib-debug/spirv-cross-glsld.lib"
Lib["SPIRV_Cross_release"] = "%{wks.location}/Eklipse/vendor/SPIRV_Cross/lib-release/spirv-cross-core.lib"
Lib["SPIRV_Cross_GLSL_release"] = "%{wks.location}/Eklipse/vendor/SPIRV_Cross/lib-release/spirv-cross-glsl.lib"

-- ScriptAPI
Include["ScriptAPI"] = "%{wks.location}/EklipseScriptAPI/src"

-- Header-only / Compiled
Include["glfw"] = "%{wks.location}/Eklipse/vendor/glfw/include"
Include["glm"] = "%{wks.location}/Eklipse/vendor/glm"
Include["glad"] = "%{wks.location}/Eklipse/vendor/glad/include"
Include["spdlog"] = "%{wks.location}/Eklipse/vendor/spdlog/include"
Include["imgui"] = "%{wks.location}/Eklipse/vendor/imgui"
Include["imguizmo"] = "%{wks.location}/Eklipse/vendor/imguizmo"
Include["stb_image"] = "%{wks.location}/Eklipse/vendor/stb_image"
Include["tiny_obj_loader"] = "%{wks.location}/Eklipse/vendor/tiny_obj_loader"
Include["vk_mem_alloc"] = "%{wks.location}/Eklipse/vendor/vk_mem_alloc"
Include["entt"] = "%{wks.location}/Eklipse/vendor/entt"
Include["nfd"] = "%{wks.location}/Eklipse/vendor/nfd/src/include"
Include["yamlcpp"] = "%{wks.location}/Eklipse/vendor/yamlcpp/include"
Include["filewatch"] = "%{wks.location}/Eklipse/vendor/filewatch"
Include["dylib"] = "%{wks.location}/Eklipse/vendor/dylib"