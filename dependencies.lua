Include = {}
Lib = {}

Include["spdlog"] = "%{wks.location}/Eklipse/vendor/spdlog/include"

Include["glfw"] = "%{wks.location}/Eklipse/vendor/GLFW/include"
Lib["glfw"] = "%{wks.location}/Eklipse/vendor/GLFW/lib-vc2022"

Include["ImGui"] = "%{wks.location}/Eklipse/vendor/imgui"

Include["stb_image"] = "%{wks.location}/Eklipse/vendor/stb_image"

VULKAN_SDK = os.getenv("VULKAN_SDK")
Include["Vulkan"] = "%{VULKAN_SDK}/Include"
Lib["Vulkan"] = "%{VULKAN_SDK}/Lib"