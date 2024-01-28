#include "precompiled.h"
#include "ImGuiExtensions.h"
#include <Eklipse/Utils/File.h>
#include <Eklipse/Assets/AssetManager.h>

bool ImGui::InputAsset(const void* id, const char* label, Eklipse::AssetType assetType, Eklipse::AssetHandle& assetHandle)
{
    ImGui::PushID(id);
    if (label)
    {
        ImGui::Text(label);
        ImGui::SameLine();
    }

    bool active = false;
    bool isCurrentlyValid = Eklipse::AssetManager::IsAssetHandleValid(assetHandle) && Eklipse::AssetManager::GetMetadata(assetHandle).Type == assetType;
    if (!isCurrentlyValid)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
    }

    ImGui::PushItemWidth(-FLT_MIN);
    if (ImGui::BeginDragDropTarget()) 
    {
        const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(label);
        if (payload)
        {
            Eklipse::AssetHandle* payloadHandlePtr = reinterpret_cast<Eklipse::AssetHandle*>(payload->Data);
            if (Eklipse::AssetManager::IsAssetHandleValid(*payloadHandlePtr) && Eklipse::AssetManager::GetMetadata(*payloadHandlePtr).Type == assetType)
            {
                assetHandle = *payloadHandlePtr;
                active = true;
            }
        }
        ImGui::EndDragDropTarget();
    }
    ImGui::PopItemWidth();

    if (!isCurrentlyValid)
    {
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
    }

    ImGui::PopID();
    return active;
}

bool ImGui::InputPath(const void* id, const char* label, Eklipse::Path& path, const std::vector<Eklipse::String>& requiredExtensions)
{
    ImGui::PushID(id);
    if (label)
    {
        ImGui::Text(label);
        ImGui::SameLine();
    }

    bool active = false;
    bool isCurrentlyValid = Eklipse::IsPathValid(path, requiredExtensions);
    if (!isCurrentlyValid)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
    }

    ImGui::PushItemWidth(-30);
    active = ImGui::InputTextWithHint("##InputPath", "Select file", (char*)path.c_str(), path.string().size());    
    ImGui::PopItemWidth();

    if (!isCurrentlyValid)
    {
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
    }

    if (active)
    {
        if (Eklipse::IsPathValid(path, requiredExtensions))
            active = true;
        else
            active = false;

    }

    ImGui::SameLine();
    if (ImGui::Button("..."))
    {
        auto& result = Eklipse::OpenFileDialog(requiredExtensions);
        if (result.type == Eklipse::FileDialogResultType::SUCCESS)
        {
            path = result.path;
            active = true;
        }
    }

    ImGui::PopID();
    return active;
}

bool ImGui::InputDir(const void* id, const char* label, Eklipse::Path& path)
{
    ImGui::PushID(id);
    if (label)
    {
        ImGui::Text(label);
        ImGui::SameLine();
    }

    bool isCurrentlyValid = Eklipse::IsPathValid(path);
    if (!isCurrentlyValid)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
    }

    ImGui::PushItemWidth(-30);
    char* pathBuffer = (char*)path.c_str();
    bool active = ImGui::InputTextWithHint("##InputPath", "Select directory", pathBuffer, path.string().size());
    ImGui::PopItemWidth();

    if (!isCurrentlyValid)
    {
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
    }

    if (active)
    {
        if (Eklipse::IsPathValid(path))
            active = true;
        else
            active = false;
    }

    ImGui::SameLine();
    if (ImGui::Button("..."))
    {
        auto& result = Eklipse::OpenDirDialog();
        if (result.type == Eklipse::FileDialogResultType::SUCCESS)
            path = result.path;
    }

    ImGui::PopID();
    return active;
}