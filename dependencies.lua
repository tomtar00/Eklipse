Include = {}
Lib = {}
VULKAN_SDK = os.getenv("VULKAN_SDK")

Include["Vulkan"] = "%{VULKAN_SDK}/Include"
Include["spdlog"] = "%{wks.location}/Eklipse/vendor/spdlog/include"
Include["glfw"] = "%{wks.location}/Eklipse/vendor/GLFW/include"
Include["ImGui"] = "%{wks.location}/Eklipse/vendor/imgui"
Include["stb_image"] = "%{wks.location}/Eklipse/vendor/stb_image"
Include["tiny_obj_loader"] = "%{wks.location}/Eklipse/vendor/tiny_obj_loader"

Lib["Vulkan"] = "%{VULKAN_SDK}/Lib"
Lib["glfw"] = "%{wks.location}/Eklipse/vendor/GLFW/lib-vc2022"