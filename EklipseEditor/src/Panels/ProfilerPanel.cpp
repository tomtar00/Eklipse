#include "ProfilerPanel.h"

namespace Editor
{
    void ProfilerPanel::OnGUI()
    {
        EK_PROFILE();

        ImGui::Begin("Profiler");
        if (ImGui::BeginTable("Profiler", 3, ImGuiTableFlags_Sortable))
        {
            ImGui::TableSetupColumn("Method", ImGuiTableColumnFlags_WidthStretch, 4.0f);
            ImGui::TableSetupColumn("Thread", ImGuiTableColumnFlags_WidthStretch, 1.0f);
            ImGui::TableSetupColumn("Time (ms)", ImGuiTableColumnFlags_WidthStretch, 1.0f);
            ImGui::TableHeadersRow();

            auto& data = Eklipse::Profiler::GetData();
            for (auto it = data.begin(); it != data.end(); it++) 
            {
                ImGui::TableNextRow();

                ImGui::TableNextColumn();
                bool xd = ImGui::CollapsingHeader(it->first);

                ImGui::TableNextColumn();
                ImGui::Text(std::to_string(it->second.threadId).c_str());

                ImGui::TableNextColumn();
                ImGui::Text(std::to_string(it->second.execTimeMs).c_str());

                if (xd)
                {
                    for (auto it2 = it->second.childNodes.begin(); it2 != it->second.childNodes.end(); it2++)
                    {
                        ImGui::TableNextRow();

                        ImGui::TableNextColumn();
                        ImGui::CollapsingHeader(it2->first);

                        ImGui::TableNextColumn();
                        ImGui::Text(std::to_string(it2->second.threadId).c_str());

                        ImGui::TableNextColumn();
                        ImGui::Text(std::to_string(it2->second.execTimeMs).c_str());
                    }
                }
            }
            ImGui::EndTable();
        }
        /*if (ImGui::CollapsingHeader("Help"))
        {
            ImGui::Text("TEST");
        }*/
        ImGui::End();
    }
}
