#include "ProfilerPanel.h"

namespace Editor
{
    void DrawTable(float indent, std::vector<Eklipse::ProfilerNode>* data, bool sortTime, uint32_t i)
    {
        if (sortTime)
        {

        }

        auto it = data->begin();
        if (indent > 0.0f) ImGui::Indent(indent);
        while (it != data->end())
        {
            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0, 0, 0, 0));
            bool expanded = false;
            if (it->ChildNodes.size() > 0) 
            {
                std::string labelBuffer = it->signature;
                labelBuffer += "##Sig" + std::to_string(++i);
                expanded = ImGui::CollapsingHeader(labelBuffer.c_str(), ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_NoAutoOpenOnLog);
            }
            else ImGui::Text(it->signature);
            ImGui::PopStyleColor(3);
            
            ImGui::TableNextColumn();
            ImGui::Text(std::to_string(it->threadId).c_str());

            ImGui::TableNextColumn();
            ImGui::Text(std::to_string(it->execTimeMs).c_str());

            if (expanded)
            {
                DrawTable(indent + 20.0f, &it->ChildNodes, sortTime, i);
            }

            ++it;
        }
        if (indent > 0.0f) ImGui::Unindent(indent);
    }

    void ProfilerPanel::OnGUI()
    {
        EK_PROFILE();

        ImGui::Begin("Profiler");
        if (ImGui::BeginTable("Profiler", 3, ImGuiTableFlags_Sortable | ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
        {
            ImGui::TableSetupColumn("Method", ImGuiTableColumnFlags_WidthStretch, 4.0f);
            ImGui::TableSetupColumn("Thread", ImGuiTableColumnFlags_WidthStretch, 1.0f);
            ImGui::TableSetupColumn("Time (ms)", ImGuiTableColumnFlags_WidthStretch, 1.0f);
            ImGui::TableHeadersRow();

            bool sortTime = false;
            auto sortSpec = ImGui::TableGetSortSpecs();
            if (sortSpec != nullptr && sortSpec->SpecsDirty)
            {
                // sort
                int columnIndex = sortSpec->Specs->ColumnIndex;
                sortTime = columnIndex == 2;
                EK_INFO("Sorting: {0}", columnIndex);
                sortSpec->SpecsDirty = false;
            }

            static uint32_t i = 0;
            std::vector<Eklipse::ProfilerNode>* data = &Eklipse::Profiler::GetLastFrameData().ProfileNodes;
            DrawTable(0.0f, data, sortTime, i);

            ImGui::EndTable();
        }
        ImGui::End();
    }
}
