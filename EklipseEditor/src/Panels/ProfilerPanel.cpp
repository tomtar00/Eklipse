#include "ProfilerPanel.h"

namespace Editor
{
    void DrawTable(float indent, std::vector<Eklipse::ProfilerNode>* data, bool ascending, int columnIndex, uint32_t i)
    {
        if (Eklipse::Profiler::CanProfile())
        {
            if (columnIndex == 1)
            {
                std::sort(data->begin(), data->end(),
                    [ascending](Eklipse::ProfilerNode const& a, Eklipse::ProfilerNode const& b)
                    {
                        return ascending ? a.threadId < b.threadId : a.threadId > b.threadId;
                    }
                );
            }
            else if (columnIndex == 2)
            {
                std::sort(data->begin(), data->end(),
                    [ascending](Eklipse::ProfilerNode const& a, Eklipse::ProfilerNode const& b)
                    {
                        return ascending ? a.numCalls < b.numCalls : a.numCalls > b.numCalls;
                    }
                );
            }
            else if (columnIndex == 3)
            {
                std::sort(data->begin(), data->end(),
                    [ascending](Eklipse::ProfilerNode const& a, Eklipse::ProfilerNode const& b)
                    { 
                        return ascending ? a.execTimeMs < b.execTimeMs : a.execTimeMs > b.execTimeMs;
                    }
                );
            } 
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

            /*if (it->ChildNodes.size() > 0) 
            {
                std::string labelBuffer = it->name;
                labelBuffer += "##Sig" + std::to_string(++i);
                expanded = ImGui::CollapsingHeader(labelBuffer.c_str(), ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_NoAutoOpenOnLog);
            }
            else ImGui::Text(it->name);*/

            std::string labelBuffer = it->name;
            labelBuffer += "##Sig" + std::to_string(++i);
            expanded = ImGui::CollapsingHeader(labelBuffer.c_str());

            ImGui::PopStyleColor(3);
            
            ImGui::TableNextColumn();
            ImGui::Text(std::to_string(it->threadId).c_str());

            ImGui::TableNextColumn();
            ImGui::Text(std::to_string(it->numCalls).c_str());

            ImGui::TableNextColumn();
            ImGui::Text(std::to_string(it->execTimeMs).c_str());

            if (expanded)
            {
                DrawTable(indent + 10.0f, &it->ChildNodes, ascending, columnIndex, i);
            }

            ++it;
        }
        if (indent > 0.0f) ImGui::Unindent(indent);
    }

    void ProfilerPanel::OnGUI()
    {
        EK_PROFILE();

        ImGui::Begin("Profiler");
        if (ImGui::BeginTable("Profiler", 4, ImGuiTableFlags_Sortable | ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
        {
            ImGui::TableSetupColumn("Method", ImGuiTableColumnFlags_WidthStretch, 4.0f);
            ImGui::TableSetupColumn("Thread", ImGuiTableColumnFlags_WidthStretch, 1.0f);
            ImGui::TableSetupColumn("Calls", ImGuiTableColumnFlags_WidthStretch, 1.0f);
            ImGui::TableSetupColumn("Time (ms)", ImGuiTableColumnFlags_WidthStretch, 1.0f);
            ImGui::TableHeadersRow();

            auto sortSpec = ImGui::TableGetSortSpecs();
            if (sortSpec != nullptr && sortSpec->SpecsDirty)
            {
                m_columnIndex = sortSpec->Specs->ColumnIndex;
                m_ascendingSort = sortSpec->Specs->SortDirection == ImGuiSortDirection_Ascending;
                sortSpec->SpecsDirty = false;
            }

            static uint32_t i = 0;
            std::vector<Eklipse::ProfilerNode>* data = &Eklipse::Profiler::GetLastFrameData().ProfileNodes;
            DrawTable(0.0f, data, m_ascendingSort, m_columnIndex, i);

            ImGui::EndTable();
        }
        ImGui::End();
    }
}
