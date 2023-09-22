#include "ProfilerPanel.h"

namespace Editor
{
    void DrawGraph(ImDrawList* drawList, ImVec2 graphPos, ImVec2 graphSize, uint32_t bgCol)
    {
        drawList->AddRectFilled(graphPos, graphSize, bgCol);

        /*ImVec2 points[4] = {
            {graphPos.x + 0, graphPos.y + 0},
            {graphPos.x + 50, graphPos.y + 20},
            {graphPos.x + 70, graphPos.y + 150},
            {graphPos.x + 200, graphPos.y + 50}
        };
        drawList->AddConvexPolyFilled(points, 4, 0xFFFFFFFF);*/

        auto& framesData = Eklipse::Profiler::GetData();
        for (uint32_t frameIdx = 0; frameIdx < MAX_PROFILED_FRAMES; frameIdx++)
        {
            float lastHeight = 0.0f;
            for (size_t nodeIdx = 0; nodeIdx < framesData[frameIdx].ProfileNodes.size(); nodeIdx++)
            {
                float height = framesData[frameIdx].ProfileNodes[nodeIdx].execTimeMs;

                float posX = frameIdx * (graphSize.x / MAX_PROFILED_FRAMES);
                float posY = lastHeight;
                float sizeX = graphSize.x / MAX_PROFILED_FRAMES;
                float sizeY = height * 10;

                EK_INFO("h:{0} pX:{1} pY:{2} sX:{3} sY:{4}", height, posX, posY, sizeX, sizeY);
                drawList->AddRectFilled({ graphPos.x + posX,  graphPos.y + posY }, { graphPos.x + sizeX, graphPos.y + sizeY }, 0xFFFFFFFF / (nodeIdx+1));

                lastHeight = height;
            }
        }
    }

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

            if (it->ChildNodes.size() > 0) 
            {
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0, 0 });
                expanded = ImGui::CollapsingHeader(it->name, ImGuiTreeNodeFlags_FramePadding);
                ImGui::PopStyleVar(1);
            }
            else 
            {
                ImGui::Indent(13.0f);
                ImGui::Text(it->name);
                ImGui::Unindent(13.0f);
            }

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

        std::vector<Eklipse::ProfilerNode>* data = &Eklipse::Profiler::GetLastFrameData().ProfileNodes;

        ImDrawList* drawList = ImGui::GetWindowDrawList();
        auto size = ImGui::GetContentRegionAvail();
        auto pos = ImGui::GetCursorScreenPos();
        DrawGraph(drawList, pos, { pos.x + size.x, pos.y + 200 }, 0x99999999);

        ImGui::SetCursorPos({ 8, 235 });
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
            DrawTable(0.0f, data, m_ascendingSort, m_columnIndex, i);

            ImGui::EndTable();
        }
        ImGui::End();
    }
}
