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

    private:
        Path m_currentPath;
        Path m_workingDirPath;

        Ref<GuiIcon> m_folderIcon;
        Ref<GuiIcon> m_fileIcon;

        //std::map<Path, AssetHandle> m_pathHandleMap;
    };
}