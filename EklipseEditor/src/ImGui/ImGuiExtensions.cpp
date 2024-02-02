#include "precompiled.h"
#include "ImGuiExtensions.h"
#include <Eklipse/Utils/File.h>
#include <Eklipse/Assets/AssetManager.h>
#include <EditorLayer.h>

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

    Eklipse::String buttonLabel = isCurrentlyValid ? 
        Eklipse::AssetManager::GetMetadata(assetHandle).FilePath.filename().string().c_str() : 
        ("Drop " + Eklipse::Asset::TypeToString(assetType) + " asset here").c_str();

    ImGui::Button(buttonLabel.c_str(), {-30, 0});
    if (ImGui::BeginDragDropTarget()) 
    {
        const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_BROWSER_ITEM");
        if (payload)
        {
            Eklipse::AssetHandle* payloadHandlePtr = reinterpret_cast<Eklipse::AssetHandle*>(payload->Data);
            if (Eklipse::AssetManager::IsAssetHandleValidAndOfType(*payloadHandlePtr, assetType))
            {
                EK_CORE_TRACE("Accepted asset handle: {0}", *payloadHandlePtr);
                assetHandle = *payloadHandlePtr;
                active = true;
            }
        }
        ImGui::EndDragDropTarget();
    }

    if (!isCurrentlyValid)
    {
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
    }

    ImGui::SameLine();
    if (ImGui::Button("..."))
    {
        auto& assetLibrary = Eklipse::EditorLayer::Get().GetAssetLibrary();
        Eklipse::Vec<Eklipse::String> requiredExtensions = Eklipse::EditorAssetLibrary::GetAssetFileExtensions(assetType);
        auto& result = Eklipse::FileUtilities::OpenFileDialog(requiredExtensions, assetLibrary->GetAssetDirectory());
        if (result.type == Eklipse::FileDialogResultType::SUCCESS)
        {
            assetHandle = assetLibrary->GetHandleFromAssetPath(result.path);
            active = true;
        }
    }

    ImGui::PopID();
    return active;
}

IMGUI_API bool ImGui::InputFilePath(const void* id, const char* label, Eklipse::Path& path, const Eklipse::Vec<Eklipse::String>& requiredExtensions, const Eklipse::Path& defaultPath)
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
        auto& result = Eklipse::FileUtilities::OpenFileDialog(requiredExtensions, defaultPath);
        if (result.type == Eklipse::FileDialogResultType::SUCCESS)
        {
            path = result.path;
            active = true;
        }
    }

    ImGui::PopID();
    return active;
}
bool ImGui::InputFilePath(const void* id, const char* label, Eklipse::Path& path, const Eklipse::Vec<Eklipse::String>& requiredExtensions)
{
    return InputFilePath(id, label, path, requiredExtensions, {});
}

bool ImGui::InputDirPath(const void* id, const char* label, Eklipse::Path& path, const Eklipse::Path& defaultPath)
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
        auto& result = Eklipse::FileUtilities::OpenDirDialog(defaultPath);
        if (result.type == Eklipse::FileDialogResultType::SUCCESS)
            path = result.path;
    }

    ImGui::PopID();
    return active;
}
bool ImGui::InputDirPath(const void* id, const char* label, Eklipse::Path& path)
{
    return InputDirPath(id, label, path, {});
}