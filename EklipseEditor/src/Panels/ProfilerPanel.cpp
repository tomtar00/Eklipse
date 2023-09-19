#include "ProfilerPanel.h"

namespace Editor
{
    void ProfilerPanel::OnGUI()
    {
        if (ImGui::BeginTable("Profiler", 3))
        {
            auto& data = Eklipse::Profiler::GetData();
            for (auto it = data.begin(); it != data.end(); it++) 
            {
                ImGui::TableNextRow();

                ImGui::TableNextColumn();
                ImGui::Text("Sig: %s", "test"/*it->first*/);

                ImGui::TableNextColumn();
                ImGui::Text("Thread: %d", 1/*it->second->threadId*/);

                ImGui::TableNextColumn();
                ImGui::Text("Ms: %f", 1/*it->second->execTimeMs*/);
            }
            ImGui::EndTable();
        }
    }
}
