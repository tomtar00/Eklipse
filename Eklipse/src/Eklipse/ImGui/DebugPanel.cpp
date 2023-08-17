#include "precompiled.h"
#ifdef EK_INCLUDE_DEBUG_LAYER
#include "DebugPanel.h"

#include <Eklipse/Core/Application.h>

namespace Eklipse
{
	void DebugPanel::OnGUI()
	{
        static float f = 0.0f;
        static int counter = 0;
        static bool test = false;
        static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        ImGui::Begin("Left");

        // ImGui::Text("This is some useful text.");
        // ImGui::Checkbox("Test", &test);
        // 
        // ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
        // ImGui::ColorEdit3("clear color", (float*)&clear_color);
        // 
        // if (ImGui::Button("Button"))
        //     counter++;
        // ImGui::SameLine();
        // ImGui::Text("counter = %d", counter);

        auto scene = Application::Get().GetScene();
        auto& camera = scene->m_camera;

        ImGui::Text("Camera");

        ImGui::SliderFloat("cam_fov", &camera.m_fov, 45.0f, 150.0f);

        ImGui::SliderFloat("cam_posx", &camera.m_transform.position.x, -10.0f, 10.0f);
        ImGui::SliderFloat("cam_posy", &camera.m_transform.position.y, -10.0f, 10.0f);
        ImGui::SliderFloat("cam_posz", &camera.m_transform.position.z, -10.0f, 10.0f);

        ImGui::SliderFloat("cam_rotx", &camera.m_transform.rotation.x, 0.0f, 360.0f);
        ImGui::SliderFloat("cam_roty", &camera.m_transform.rotation.y, 0.0f, 360.0f);
        ImGui::SliderFloat("cam_rotz", &camera.m_transform.rotation.z, 0.0f, 360.0f);

        int i = 0;
        for (auto& model : scene->m_geometry)
        {
            ImGui::Text("Model");
            std::stringstream title;
            title << "model-" << i;

            ImGui::SliderFloat((title.str() + "posx").c_str(), &model.m_transform.position.x, -10.0f, 10.0f);
            ImGui::SliderFloat((title.str() + "posy").c_str(), &model.m_transform.position.y, -10.0f, 10.0f);
            ImGui::SliderFloat((title.str() + "posz").c_str(), &model.m_transform.position.z, -10.0f, 10.0f);

            ImGui::SliderFloat((title.str() + "rotx").c_str(), &model.m_transform.rotation.x, 0.0f, 360.0f);
            ImGui::SliderFloat((title.str() + "roty").c_str(), &model.m_transform.rotation.y, 0.0f, 360.0f);
            ImGui::SliderFloat((title.str() + "rotz").c_str(), &model.m_transform.rotation.z, 0.0f, 360.0f);

            ImGui::SliderFloat((title.str() + "scalex").c_str(), &model.m_transform.scale.x, 0.1f, 2.0f);
            ImGui::SliderFloat((title.str() + "scaley").c_str(), &model.m_transform.scale.y, 0.1f, 2.0f);
            ImGui::SliderFloat((title.str() + "scalez").c_str(), &model.m_transform.scale.z, 0.1f, 2.0f);

            ++i;
        }

        ImGui::End();
	}
}
#endif