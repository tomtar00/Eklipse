#pragma once

#include "AssetLibrary.h"
#include "AssetMetadata.h"

#include <FileWatch.hpp>

#define EK_REGISTRY_EXTENSION ".ekreg"

namespace Eklipse 
{
    using AssetRegistry = std::map<AssetHandle, AssetMetadata>;

    class EK_API EditorAssetLibrary : public AssetLibrary
    {
    public:
        EditorAssetLibrary(const Path& assetDirectory);
        ~EditorAssetLibrary();

        virtual Ref<Asset> GetAsset(AssetHandle handle) override;
        virtual AssetMetadata& GetMetadata(AssetHandle handle) override;
        virtual bool IsAssetHandleValid(AssetHandle handle) const override;
        virtual bool IsAssetLoaded(AssetHandle handle) const override;

        virtual void UnloadAssets() override;
        virtual void ReloadAssets() override;

        AssetRegistry& GetAssetRegistry();
        Path& GetAssetDirectory();
        virtual AssetHandle ImportAsset(const Path& filepath) override;
        AssetHandle ImportDefaultMaterial(const Path& filepath, AssetHandle shaderHandle);
        void RemoveAsset(AssetHandle handle);
        AssetHandle GetHandleFromAssetPath(const Path& path, bool reqExists = true) const;
        void Validate();

        static bool SerializeAssetRegistry(const AssetRegistry& registry, const Path& filepath);
        static bool DeserializeAssetRegistry(AssetRegistry& registry, const Path& filepath);
        bool SerializeAssetRegistry();
        bool DeserializeAssetRegistry();

        void StartFileWatcher();
        void OnFileWatchEvent(const String& path, filewatch::Event change_type);

    private:
        AssetRegistry m_assetRegistry;
        Path m_assetDirectory;

        Unique<filewatch::FileWatch<String>> m_fileWatcher;

        bool m_shaderReloadPending;
        AssetHandle m_renamedAssetHandle;
    };
}