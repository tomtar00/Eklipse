Include = {}
Lib = {}

Include["Vulkan"] = "%{wks.location}/Eklipse/vendor/Vulkan/include"
Lib["Vulkan"] = "%{wks.location}/Eklipse/vendor/Vulkan/lib/vulkan-1.lib"

Include["glfw"] = "%{wks.location}/Eklipse/vendor/GLFW/include"
Lib["glfw3"] = "%{wks.location}/Eklipse/vendor/GLFW/lib-vc2022/glfw3.lib"
Lib["glfw3_mt"] = "%{wks.location}/Eklipse/vendor/GLFW/lib-vc2022/glfw3_mt.lib"
Lib["glfw3dll"] = "%{wks.location}/Eklipse/vendor/GLFW/lib-vc2022/glfw3dll.lib"

Include["shaderc"] = "%{wks.location}/Eklipse/vendor/shaderc/include"
Lib["shaderc_debug"] = "%{wks.location}/Eklipse/vendor/shaderc/lib-debug/shaderc_sharedd.lib"
Lib["shaderc_release"] = "%{wks.location}/Eklipse/vendor/shaderc/lib-release/shaderc_shared.lib"

Include["SPIRV_Cross"] = "%{wks.location}/Eklipse/vendor/SPIRV_Cross/include"
Lib["SPIRV_Cross_debug"] = "%{wks.location}/Eklipse/vendor/SPIRV_Cross/lib-debug/spirv-cross-cored.lib"
Lib["SPIRV_Cross_GLSL_debug"] = "%{wks.location}/Eklipse/vendor/SPIRV_Cross/lib-debug/spirv-cross-glsld.lib"
Lib["SPIRV_Cross_release"] = "%{wks.location}/Eklipse/vendor/SPIRV_Cross/lib-release/spirv-cross-core.lib"
Lib["SPIRV_Cross_GLSL_release"] = "%{wks.location}/Eklipse/vendor/SPIRV_Cross/lib-release/spirv-cross-glsl.lib"

Include["glm"] = "%{wks.location}/Eklipse/vendor/glm"
Include["spdlog"] = "%{wks.location}/Eklipse/vendor/spdlog/include"
Include["Glad"] = "%{wks.location}/Eklipse/vendor/Glad/include"
Include["ImGui"] = "%{wks.location}/Eklipse/vendor/imgui"
Include["ImGuizmo"] = "%{wks.location}/Eklipse/vendor/ImGuizmo"
Include["stb_image"] = "%{wks.location}/Eklipse/vendor/stb_image"
Include["tiny_obj_loader"] = "%{wks.location}/Eklipse/vendor/tiny_obj_loader"
Include["vk_mem_alloc"] = "%{wks.location}/Eklipse/vendor/vk_mem_alloc"
Include["entt"] = "%{wks.location}/Eklipse/vendor/entt"
Include["nativefiledialog"] = "%{wks.location}/Eklipse/vendor/nativefiledialog/src/include"
Include["yaml_cpp"] = "%{wks.location}/Eklipse/vendor/yaml-cpp/include"
Include["filewatch"] = "%{wks.location}/Eklipse/vendor/filewatch"