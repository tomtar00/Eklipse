Include = {}
Lib = {}
VULKAN_SDK = os.getenv("VULKAN_SDK")

Include["Vulkan"] = "%{VULKAN_SDK}/Include"
Include["spdlog"] = "%{wks.location}/Eklipse/vendor/spdlog/include"
Include["glfw"] = "%{wks.location}/Eklipse/vendor/GLFW/include"
Include["Glad"] = "%{wks.location}/Eklipse/vendor/Glad/include"
Include["ImGui"] = "%{wks.location}/Eklipse/vendor/imgui"
Include["stb_image"] = "%{wks.location}/Eklipse/vendor/stb_image"
Include["tiny_obj_loader"] = "%{wks.location}/Eklipse/vendor/tiny_obj_loader"
Include["vk_mem_alloc"] = "%{wks.location}/Eklipse/vendor/vk_mem_alloc"

Lib["Vulkan"] = "%{VULKAN_SDK}/Lib"
Lib["glfw"] = "%{wks.location}/Eklipse/vendor/GLFW/lib-vc2022"