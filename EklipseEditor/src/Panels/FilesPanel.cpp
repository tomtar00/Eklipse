#include "FilesPanel.h"
#include "EditorLayer.h"
#include <Eklipse/Project/Project.h>
#include <misc/cpp/imgui_stdlib.h>

namespace Eklipse
{
    static String ShaderTemplatePathFromOption(int option)
    {
        switch (option)
        {
        case 0:
            return "Assets/Shaders/3D.glsl";
        case 1:
            return "Assets/Shaders/2D.glsl";
        default:
            return "";
        }
    }

    void FilesPanel::LoadResources()
    {
        m_folderIcon = GuiIcon::Create("Assets/Icons/folder.png");
        m_fileIcon = GuiIcon::Create("Assets/Icons/file.png");
        m_materialIcon = GuiIcon::Create("Assets/Icons/material.png");
        m_shaderIcon = GuiIcon::Create("Assets/Icons/shader.png");
        m_sceneIcon = GuiIcon::Create("Assets/Icons/scene.png");
        m_textureIcon = GuiIcon::Create("Assets/Icons/texture.png");
        m_meshIcon = GuiIcon::Create("Assets/Icons/mesh.png");
    }
    void FilesPanel::UnloadResources()
    {
        m_folderIcon->Dispose();
        m_fileIcon->Dispose();
        m_materialIcon->Dispose();
        m_shaderIcon->Dispose();
        m_sceneIcon->Dispose();
        m_textureIcon->Dispose();
        m_meshIcon->Dispose();
    }
    void FilesPanel::OnContextChanged()
    {
        m_workingDirPath = Project::GetActive()->GetConfig().assetsDirectoryPath;
        m_currentPath = m_workingDirPath;

        /*for (auto& [handle, metadata] : EditorLayer::Get().GetAssetLibrary()->GetAssetRegistry())
        {
            m_pathHandleMap[metadata.FilePath] = handle;
        }*/
    }

    bool FilesPanel::OnGUI(float deltaTime)
    {
        EK_PROFILE();

        if (!GuiPanel::OnGUI(deltaTime)) return false;

        ImGui::Begin("Files");
        if (m_workingDirPath.empty())
        {
            ImGui::Text("No project loaded");
            ImGui::SameLine();
            if (ImGui::Button("Open Project"))
            {
                EditorLayer::Get().OpenProject();
            }
            ImGui::End();
            return true;
        }

        // Breadcrumbs
        {
            Vec<Path> paths;
            Path path = m_currentPath;
            while (path != Path(m_workingDirPath).parent_path())
            {
                paths.push_back(path);
                path = path.parent_path();
            }

            for (int i = (int)paths.size() - 1; i >= 0; i--)
            {
                ImGui::Text("/");
                ImGui::SameLine();
                if (ImGui::Button(paths[i].filename().string().c_str()))
                {
                    m_currentPath = paths[i];
                }
                ImGui::SameLine();
            }
        }

        ImGui::NewLine();
        //ImGui::Separator();

        static float padding = 32.0f;
        static float thumbnailSize = 64.0f;
        float cellSize = thumbnailSize + padding;

        float panelWidth = ImGui::GetContentRegionAvail().x;
        int columnCount = (int)(panelWidth / cellSize);
        if (columnCount < 1) columnCount = 1;

        ImGui::Columns(columnCount, 0, false);

        for (auto& directoryEntry : std::filesystem::directory_iterator(m_currentPath))
        {
            const auto& path = directoryEntry.path();
            String filename = path.filename().string();

            ImGui::PushID(filename.c_str());
            Ref<GuiIcon> icon = GetIcon(path);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::ImageButton((ImTextureID)icon->GetID(), { thumbnailSize, thumbnailSize });

            if (ImGui::BeginDragDropSource())
            {
                AssetHandle assetHandle = EditorLayer::Get().GetAssetLibrary()->GetHandleFromAssetPath(m_currentPath / path);
                ImGui::SetDragDropPayload("ASSET_BROWSER_ITEM", &assetHandle, sizeof(AssetHandle*), ImGuiCond_Once);
                ImGui::EndDragDropSource();
            }

            ImGui::PopStyleColor();

            if (ImGui::BeginPopupContextItem("ItemMenu"))
            {
                if (ImGui::Selectable("Copy Path"))
                {
                    ImGui::SetClipboardText((m_currentPath / filename).string().c_str());
                }
                ImGui::EndPopup();
            }

            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            {
                if (directoryEntry.is_directory())
                    m_currentPath /= path.filename();

                // TODO: open scene
            }

            if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left) && !ImGui::IsMouseDragging(ImGuiMouseButton_Left) && path.has_extension())
            {
                AssetType type = EditorAssetLibrary::GetAssetTypeFromFileExtension(path.extension().string());
                if (type == AssetType::Material)
                {
                    AssetHandle materialHandle = EditorLayer::Get().GetAssetLibrary()->GetHandleFromAssetPath(m_currentPath / path);
                    if (AssetManager::IsAssetHandleValid(materialHandle))
                    {
                        DetailsSelectionInfo info{};
                        info.type = SelectionType::MATERIAL;
                        info.material = AssetManager::GetAsset<Material>(materialHandle).get();
                        EditorLayer::Get().SelectionInfo = info;
                    }
                }
                else if (type == AssetType::Shader)
                {
                    AssetHandle shaderHandle = EditorLayer::Get().GetAssetLibrary()->GetHandleFromAssetPath(m_currentPath / path);
                    if (AssetManager::IsAssetHandleValid(shaderHandle))
                    {
                        DetailsSelectionInfo info{};
                        info.type = SelectionType::SHADER;
                        info.shader = AssetManager::GetAsset<Shader>(shaderHandle).get();
                        EditorLayer::Get().SelectionInfo = info;
                    }
                }
            }

            ImGui::Text(filename.c_str());
            ImGui::NextColumn();
            ImGui::PopID();
        }

        ImGui::Columns(1);
        //ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16, 128);

        // Right click menu
        static char* popupName = nullptr;
        if (ImGui::BeginPopupContextWindow("Create", ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_MouseButtonRight))
        {
            if (ImGui::MenuItem("Create Folder"))
            {
                popupName = "Create New Folder";
            }
            if (ImGui::MenuItem("Create Shader"))
            {
                popupName = "Create New Shader";
            }
            if (ImGui::MenuItem("Create Material"))
            {
                popupName = "Create New Material";
            }
            if (ImGui::MenuItem("Import Asset"))
            {
                ImportAsset();
            }

            ImGui::EndPopup();
        }

        // Popups creation
        {
            if (popupName != nullptr)
            {
                ImGui::OpenPopup(popupName);
                popupName = nullptr;
            }

            // create folder
            if (ImGui::BeginPopupModal("Create New Folder", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
            {
                static String folderName = "NewFolder";

                ImGui::InputText("Folder Name", &folderName);

                if (ImGui::Button("Create") && !folderName.empty())
                {
                    std::filesystem::create_directory(m_currentPath / folderName);
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (ImGui::Button("Cancel"))
                {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }

            if (ImGui::BeginPopupModal("Create New Shader", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
            {
                static String shaderName = "NewShader";
                static int templateOption = true;

                ImGui::InputText("Shader Name", &shaderName);
                ImGui::RadioButton("3D", &templateOption, 0);
                ImGui::SameLine();
                ImGui::RadioButton("2D", &templateOption, 1);

                if (ImGui::Button("Create") && CreateShader(shaderName, templateOption))
                {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (ImGui::Button("Cancel"))
                {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }

            if (ImGui::BeginPopupModal("Create New Material", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
            {
                static String materialName = "NewMaterial";
                static AssetHandle shaderHandle = 0;

                ImGui::TextUnformatted("Material Name");
                ImGui::SameLine();
                ImGui::InputText("##materialName", &materialName);
                ImGui::InputAsset("##shaderid", "Shader", AssetType::Shader, shaderHandle);

                if (ImGui::Button("Create") && CreateMaterial(materialName, shaderHandle))
                {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (ImGui::Button("Cancel"))
                {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        }

        ImGui::End();
        return true;
    }
    
    bool FilesPanel::CreateMaterial(const String& materialName, const AssetHandle shaderHandle) const
    {
        if (materialName.empty())
        {
            EK_CORE_ERROR("Material name is empty");
            return false;
        }
        if (!AssetManager::IsAssetHandleValidAndOfType(shaderHandle, AssetType::Shader)) 
        {
            EK_CORE_ERROR("Invalid shader handle");
            return false;
        }
        
        String name = FileUtilities::AppendExtensionIfNotPresent(materialName, EK_MATERIAL_EXTENSION);
        Path materialPath = m_currentPath / name;

        // serialization
        Ref<Material> material = Material::Create(materialPath, shaderHandle);

        // the editor asset library will import the shader automatically

        return true;
    }
    bool FilesPanel::CreateShader(const String& shaderName, const uint32_t templateOption) const
    {
        if (shaderName.empty())
        {
            EK_CORE_ERROR("Shader name is empty");
            return false;
        }
        if (templateOption < 0)
        {
            EK_CORE_ERROR("Invalid template option");
            return false;
        }

        Path templatePath = ShaderTemplatePathFromOption(templateOption);
        String name = FileUtilities::AppendExtensionIfNotPresent(shaderName, EK_SHADER_EXTENSION);
        Path shaderPath = m_currentPath / name;

        if(!fs::copy_file(templatePath, shaderPath, fs::copy_options::skip_existing))
        {
            EK_CORE_ERROR("Failed to create shader from template '{}' to path '{}'", templatePath.string(), shaderPath.string());
            return false;
        }

        // the editor asset library will import the shader automatically

        return true;
    }
    bool FilesPanel::ImportAsset() const
    {
        FileDialogResult result = FileUtilities::OpenFileDialog();
        if (result.type == FileDialogResultType::SUCCESS)
        {
            Path filePath = result.path;
            Path destPath = m_currentPath / result.path.filename();
            if (!fs::copy_file(filePath, destPath, fs::copy_options::skip_existing))
            {
                EK_CORE_ERROR("Failed to import asset from '{}' to path '{}'", filePath.string(), destPath.string());
                return false;
            }

            // the editor asset library will import the shader automatically
        }
        return false;
    }
    Ref<GuiIcon> FilesPanel::GetIcon(const Path& path) const
    {
        if (!fs::is_directory(path))
        {
            String extension = path.extension().string();
            AssetType type = EditorAssetLibrary::GetAssetTypeFromFileExtension(extension);

            if (type == AssetType::Material)
                return m_materialIcon;
            if (type == AssetType::Shader)
                return m_shaderIcon;
            if (type == AssetType::Scene)
                return m_sceneIcon;
            if (type == AssetType::Texture2D)
                return m_textureIcon;
            if (type == AssetType::Mesh)
                return m_meshIcon;
            
            return m_fileIcon;
        }
        else
        {
            return m_folderIcon;
        }
    }
}