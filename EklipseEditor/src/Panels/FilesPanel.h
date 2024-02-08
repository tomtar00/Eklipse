#pragma once
#include <Eklipse.h>
#include <filesystem>
#include <Eklipse/Renderer/Texture.h>
#include <Eklipse/Assets/AssetManager.h>

namespace Eklipse
{
    class FilesPanel : public GuiPanel
    {
    public:
        void LoadResources();
        void UnloadResources();
        void OnContextChanged();

        bool OnGUI(float deltaTime) override;

    private:
        bool CreateMaterial(const String& materialName, const AssetHandle shaderHandle) const;
        bool CreateShader(const String& shaderName, const uint32_t templateOption) const;
        bool ImportAsset() const;

        Ref<GuiIcon> GetIcon(const Path& path) const;

    private:
        Path m_currentPath;
        Path m_workingDirPath;

        Ref<GuiIcon> m_folderIcon;
        Ref<GuiIcon> m_fileIcon;
        Ref<GuiIcon> m_materialIcon;
        Ref<GuiIcon> m_meshIcon;
        Ref<GuiIcon> m_shaderIcon;
        Ref<GuiIcon> m_textureIcon;
        Ref<GuiIcon> m_sceneIcon;

        //std::map<Path, AssetHandle> m_pathHandleMap;
    };
}