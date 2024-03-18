#include "precompiled.h"
#include "ImGuiExtensions.h"
#include <Eklipse/Utils/File.h>
#include <Eklipse/Assets/AssetManager.h>
#include <EditorLayer.h>

#define EK_INPUT_MARGIN_END 35.0f
#define EK_OPTION_BUTTON_WIDTH 30.0f

bool ImGui::InputAsset(const void* id, const char* label, Eklipse::AssetType assetType, Eklipse::AssetHandle& assetHandle)
{
    bool active = false;
    DrawProperty(id, label, [&]() {

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 5));

        bool isCurrentlyValid = Eklipse::AssetManager::IsAssetHandleValidAndOfType(assetHandle, assetType);
        if (!isCurrentlyValid)
        {
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
        }

        Eklipse::String buttonLabel = isCurrentlyValid ? 
            Eklipse::AssetManager::GetMetadata(assetHandle).FilePath.filename().string().c_str() : 
            ("None (" + Eklipse::Asset::TypeToString(assetType) + ")").c_str();


        ImGui::Button(buttonLabel.c_str(), { -EK_INPUT_MARGIN_END, 0 });
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
        if (ImGui::Button("...", { EK_OPTION_BUTTON_WIDTH, 0 }))
        {
            auto& assetLibrary = Eklipse::EditorLayer::Get().GetAssetLibrary();
            Eklipse::Vec<Eklipse::String> requiredExtensions = Eklipse::Asset::GetFileExtensions(assetType);
            auto& result = Eklipse::FileUtilities::OpenFileDialog(requiredExtensions, assetLibrary->GetAssetDirectory());
            if (result.type == Eklipse::FileDialogResultType::SUCCESS)
            {
                assetHandle = assetLibrary->GetHandleFromAssetPath(result.path);
                active = true;
            }
        }

        ImGui::PopStyleVar();

    });
    return active;
}

IMGUI_API bool ImGui::InputFilePath(const void* id, const char* label, Eklipse::Path& path, const Eklipse::Vec<Eklipse::String>& requiredExtensions, const Eklipse::Path& defaultPath)
{
    bool active = false;
    DrawProperty(id, label, [&]() {

        bool isCurrentlyValid = Eklipse::FileUtilities::IsPathValid(path, requiredExtensions);
        if (!isCurrentlyValid)
        {
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
        }

        static char pathBuffer[512];
        strncpy(pathBuffer, path.string().c_str(), sizeof(pathBuffer));

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 5));

        ImGui::PushItemWidth(-EK_INPUT_MARGIN_END);
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
        if (ImGui::Button("...", { EK_OPTION_BUTTON_WIDTH, 0 }))
        {
            auto& result = Eklipse::FileUtilities::OpenFileDialog(requiredExtensions, defaultPath);
            if (result.type == Eklipse::FileDialogResultType::SUCCESS)
            {
                path = result.path;
                active = true;
            }
        }

        ImGui::PopStyleVar();

    });
    return active;
}
bool ImGui::InputFilePath(const void* id, const char* label, Eklipse::Path& path, const Eklipse::Vec<Eklipse::String>& requiredExtensions)
{
    return InputFilePath(id, label, path, requiredExtensions, {});
}

bool ImGui::InputDirPath(const void* id, const char* label, Eklipse::Path& path, const Eklipse::Path& defaultPath)
{
    bool active = false;
    DrawProperty(id, label, [&]() {

        bool isCurrentlyValid = Eklipse::FileUtilities::IsPathValid(path);
        if (!isCurrentlyValid)
        {
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
        }

        static char pathBuffer[512];
        strncpy(pathBuffer, path.string().c_str(), sizeof(pathBuffer));

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 5));

        ImGui::PushItemWidth(-EK_INPUT_MARGIN_END);
        active = ImGui::InputTextWithHint("##InputPath", "Select directory", pathBuffer, sizeof(pathBuffer));
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
        if (ImGui::Button("...", { EK_OPTION_BUTTON_WIDTH, 0 }))
        {
            auto& result = Eklipse::FileUtilities::OpenDirDialog(defaultPath);
            if (result.type == Eklipse::FileDialogResultType::SUCCESS)
                path = result.path;
        }

        ImGui::PopStyleVar();

    });
    return active;
}
bool ImGui::InputDirPath(const void* id, const char* label, Eklipse::Path& path)
{
    return InputDirPath(id, label, path, {});
}

void ImGui::InputVec3(const void* id, const char* label, float labelWidth, glm::vec3& vec, float speed, float resetValue)
{
    DrawProperty(id, label, [&]() {

        auto& style = ImGui::GetStyle();
        auto boldFont = ImGui::GetIO().Fonts->Fonts[0];
        ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());

        float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
        ImVec2 buttonSize = ImVec2(lineHeight - 7.0f, lineHeight);

        float inputWidth = (ImGui::GetContentRegionAvail().x - 3 * (buttonSize.x) - 2 * style.ItemSpacing.x) / 3;

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, style.FramePadding.y));
        ImGui::PushStyleColor(ImGuiCol_Button, EK_COLOR_LIGHT_RED);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, EK_COLOR_RED);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, EK_COLOR_DARK_RED);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
        ImGui::PushFont(boldFont);
        if (ImGui::Button("X", buttonSize))
        {
            vec.x = resetValue;
        }
        ImGui::PopFont();
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(4);

        ImGui::SameLine(0, 0);
        ImGui::PushItemWidth(inputWidth);
        ImGui::DragFloat("##X", &vec.x, speed);
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, style.FramePadding.y));
        ImGui::PushStyleColor(ImGuiCol_Button, EK_COLOR_LIGHT_GREEN);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, EK_COLOR_GREEN);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, EK_COLOR_DARK_GREEN);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
        ImGui::PushFont(boldFont);
        if (ImGui::Button("Y", buttonSize))
        {
            vec.y = resetValue;
        }
        ImGui::PopFont();
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(4);

        ImGui::SameLine(0, 0);
        ImGui::PushItemWidth(inputWidth);
        ImGui::DragFloat("##Y", &vec.y, speed);
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, style.FramePadding.y));
        ImGui::PushStyleColor(ImGuiCol_Button, EK_COLOR_LIGHT_BLUE);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, EK_COLOR_BLUE);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, EK_COLOR_DARK_BLUE);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
        ImGui::PushFont(boldFont);
        if (ImGui::Button("Z", buttonSize))
        {
            vec.z = resetValue;
        }
        ImGui::PopFont();
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(4);

        ImGui::SameLine(0, 0);
        ImGui::PushItemWidth(inputWidth);
        ImGui::DragFloat("##Z", &vec.z, speed);
        ImGui::PopItemWidth();

    }, false);
}