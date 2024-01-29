#include "ProfilerPanel.h"

namespace Eklipse
{
    static ImColor GetColorByIndex(size_t index)
    {
        switch (index)
        {
            case 0: return ImColor(255, 128, 0);   // orange
            case 1: return ImColor(128, 255, 0);   // lime
            case 2: return ImColor(0, 255, 255);   // cyan
            case 3: return ImColor(128, 0, 255);   // purple
            case 4: return ImColor(0, 64, 255);    // blue
            default: return 0xFFFFFFFF;
        }
    }
    static void DrawGraph(ImDrawList* drawList, ImVec2 graphPos, ImVec2 graphSize, float labelWidth, Vec<ProfilerNode>& lastestFrame, Vec<ProfilerFrameData>& framesData)
    {
        drawList->AddRectFilled(graphPos, { graphPos.x + graphSize.x, graphPos.y + graphSize.y }, 0x55555555);

        const size_t labelCount = lastestFrame.size();
        Vec<ImVec2> lastFrameGlobalPositions;
        Vec<float> lastFrameHeights;
        float barWidth = graphSize.x / MAX_PROFILED_FRAMES;
        float spacingX = barWidth / 2.0f;
        float spacingY = 5.0f;

        float height = 0.0f, lastHeight = 0.0f;
        static float heightRatio = -1.0f;
        ImVec2 localPos, localSize, globalPos, globalSize;

        if (Profiler::IsProfilingCurrentFrame())
        {
            heightRatio = -1.0f;
            for (uint32_t frameIdx = 0; frameIdx < MAX_PROFILED_FRAMES; frameIdx++)
            {
                float height = 0.0f;
                for (size_t nodeIdx = 0; nodeIdx < framesData[frameIdx].ProfileNodes.size(); nodeIdx++)
                {
                    height += framesData[frameIdx].ProfileNodes[nodeIdx].execTimeMs;
                }
                if (height > 0.0f)
                {
                    float ratio = (graphSize.y - spacingY * framesData[frameIdx].ProfileNodes.size()) / height;
                    if (heightRatio < 0.0f || ratio < heightRatio)
                    {
                        heightRatio = ratio;
                    }
                }    
            }
        }

        for (uint32_t frameIdx = 0; frameIdx < MAX_PROFILED_FRAMES; frameIdx++)
        {
            lastHeight = 0.0f;
            for (size_t nodeIdx = 0; nodeIdx < framesData[frameIdx].ProfileNodes.size(); nodeIdx++)
            {
                auto& node = framesData[frameIdx].ProfileNodes[nodeIdx];
                height = node.execTimeMs * heightRatio;

                localPos = { frameIdx * barWidth + spacingX, lastHeight };
                localSize = { barWidth - spacingX, height };

                globalPos = { graphPos.x + localPos.x, graphPos.y + (graphSize.y - localPos.y) };
                globalSize = { globalPos.x + localSize.x, globalPos.y - localSize.y };

                drawList->AddRectFilled(globalPos, globalSize, GetColorByIndex(nodeIdx));

                lastHeight += height + spacingY;

                if (frameIdx == MAX_PROFILED_FRAMES - 1)
                {
                    lastFrameGlobalPositions.push_back(globalPos);
                    lastFrameHeights.push_back(height);
                }
            }
        }

        height = 20;
        spacingX = 20;
        ImVec2 lastNodeGlobalPos;
        ImVec2 p1, p2;
        for (size_t nodeIdx = 0; nodeIdx < lastestFrame.size(); nodeIdx++)
        {
            ImColor color = GetColorByIndex(nodeIdx);
            lastNodeGlobalPos = lastFrameGlobalPositions[nodeIdx];

            localPos = { spacingX, graphSize.y - height * (nodeIdx+1) };
            globalPos = { graphPos.x + graphSize.x + localPos.x, graphPos.y + localPos.y };
            drawList->AddText(globalPos, color, lastestFrame[nodeIdx].name);

            if (lastFrameHeights[nodeIdx] >= 0.2f)
            {
                globalPos = { globalPos.x + spacingX, globalPos.y + height - 5.0f };
                p1 = { globalPos.x - spacingX - 5.0f, globalPos.y };
                p2 = { lastNodeGlobalPos.x + barWidth, lastNodeGlobalPos.y + spacingY / 2.0f };

                drawList->AddLine(globalPos, p1, color);
                drawList->AddLine(p1, p2, color);
                drawList->AddLine(p2, { p2.x - barWidth, p2.y }, color);
            }
        }
    }
    static void DrawTable(float indent, Vec<ProfilerNode>& data, bool ascending, int columnIndex, uint32_t i)
    {
        if (Profiler::IsProfilingCurrentFrame())
        {
            if (columnIndex == 1)
            {
                std::sort(data.begin(), data.end(),
                    [ascending](ProfilerNode const& a, ProfilerNode const& b)
                    {
                        return ascending ? a.threadId < b.threadId : a.threadId > b.threadId;
                    }
                );
            }
            else if (columnIndex == 2)
            {
                std::sort(data.begin(), data.end(),
                    [ascending](ProfilerNode const& a, ProfilerNode const& b)
                    {
                        return ascending ? a.numCalls < b.numCalls : a.numCalls > b.numCalls;
                    }
                );
            }
            else if (columnIndex == 3)
            {
                std::sort(data.begin(), data.end(),
                    [ascending](ProfilerNode const& a, ProfilerNode const& b)
                    { 
                        return ascending ? a.execTimeMs < b.execTimeMs : a.execTimeMs > b.execTimeMs;
                    }
                );
            } 
        }

        auto it = data.begin();
        if (indent > 0.0f) ImGui::Indent(indent);
        while (it != data.end())
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
                DrawTable(indent + 10.0f, it->ChildNodes, ascending, columnIndex, i);
            }

            ++it;
        }
        if (indent > 0.0f) ImGui::Unindent(indent);
    }

    bool ProfilerPanel::OnGUI(float deltaTime)
    {
        EK_PROFILE();

        if (!GuiPanel::OnGUI(deltaTime)) return false;

        ImGui::Begin("Profiler");

        static Vec<ProfilerNode> data{};
        static Vec<ProfilerFrameData> framesData{};
        if (Profiler::IsProfilingCurrentFrame())
        {
            data = Profiler::GetLastFrameData().ProfileNodes;
            framesData = Profiler::GetData();
        }

        ImDrawList* drawList = ImGui::GetWindowDrawList();
        auto size = ImGui::GetContentRegionAvail();
        auto pos = ImGui::GetCursorScreenPos();
        static float labelWidth = 200.0f;
        static float graphHeight = 200.0f;
        DrawGraph(drawList, pos, { size.x - labelWidth, graphHeight }, labelWidth, Profiler::GetLastFrameData().ProfileNodes, Profiler::GetData());

        ImGui::SetCursorPos({ 8.0f, graphHeight + 35.0f });
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

        return true;
    }
}
