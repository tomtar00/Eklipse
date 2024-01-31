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
    bool isCurrentlyValid = Eklipse::AssetManager::IsAssetHandleValidAndOfType(assetHandle, assetType);
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
            if (Eklipse::AssetManager::IsAssetHandleValidAndOfType(*payloadHandlePtr, assetType))
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

bool ImGui::InputFilePath(const void* id, const char* label, Eklipse::Path& path, const Eklipse::Vec<Eklipse::String>& requiredExtensions)
{
    ImGui::PushID(id);
    if (label)
    {
        ImGui::Text(label);
        ImGui::SameLine();
    }

    bool active = false;
    bool isCurrentlyValid = Eklipse::FileUtilities::IsPathValid(path, requiredExtensions);
    if (!isCurrentlyValid)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
    }

    static char pathBuffer[512];
    strncpy(pathBuffer, path.string().c_str(), sizeof(pathBuffer));

    ImGui::PushItemWidth(-30);
    active = ImGui::InputTextWithHint("##InputPath", "Select file", pathBuffer, sizeof(pathBuffer));
    ImGui::PopItemWidth();

    if (!isCurrentlyValid)
    {
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
    }

    if (active)
    {
        active = Eklipse::FileUtilities::IsPathValid(path, requiredExtensions);
        path = Eklipse::Path(pathBuffer);
    }

    ImGui::SameLine();
    if (ImGui::Button("..."))
    {
        auto& result = Eklipse::FileUtilities::OpenFileDialog(requiredExtensions);
        if (result.type == Eklipse::FileDialogResultType::SUCCESS)
        {
            path = result.path;
            active = true;
        }
    }

    ImGui::PopID();
    return active;
}

bool ImGui::InputDirPath(const void* id, const char* label, Eklipse::Path& path)
{
    ImGui::PushID(id);
    if (label)
    {
        ImGui::Text(label);
        ImGui::SameLine();
    }

    bool isCurrentlyValid = Eklipse::FileUtilities::IsPathValid(path);
    if (!isCurrentlyValid)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
    }

    static char pathBuffer[512];
    strncpy(pathBuffer, path.string().c_str(), sizeof(pathBuffer));

    ImGui::PushItemWidth(-30);
    bool active = ImGui::InputTextWithHint("##InputPath", "Select directory", pathBuffer, sizeof(pathBuffer));
    ImGui::PopItemWidth();

    if (!isCurrentlyValid)
    {
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
    }

    if (active)
    {
        active = Eklipse::FileUtilities::IsPathValid(path);
        path = Eklipse::Path(pathBuffer);
    }

    ImGui::SameLine();
    if (ImGui::Button("..."))
    {
        auto& result = Eklipse::FileUtilities::OpenDirDialog();
        if (result.type == Eklipse::FileDialogResultType::SUCCESS)
            path = result.path;
    }

    ImGui::PopID();
    return active;
}