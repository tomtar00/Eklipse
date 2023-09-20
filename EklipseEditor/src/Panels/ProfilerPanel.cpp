#include "ProfilerPanel.h"

namespace Editor
{
    using ProfileData = std::unordered_map<const char*, Eklipse::ProfileNode>;

    void DrawTable(float indent, ProfileData* data, bool sortTime)
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
            if (it->second.childNodes.size() > 0) 
            {
                expanded = ImGui::CollapsingHeader(it->first, ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_NoAutoOpenOnLog);
            }
            else ImGui::Text(it->first);
            ImGui::PopStyleColor(3);
            
            ImGui::TableNextColumn();
            ImGui::Text(std::to_string(it->second.threadId).c_str());

            ImGui::TableNextColumn();
            ImGui::Text(std::to_string(it->second.execTimeMs).c_str());

            if (expanded)
            {
                DrawTable(indent + 20.0f, &it->second.childNodes, sortTime);
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

            ProfileData* data = &Eklipse::Profiler::GetData();
            DrawTable(0.0f, data, sortTime);

            ImGui::EndTable();
        }
        ImGui::End();
    }
}
