#pragma once

#include "AssetLibrary.h"
#include "AssetMetadata.h"

#include <FileWatch.hpp>

#define EK_MATERIAL_EXTENSION ".ekmt"
#define EK_SCENE_EXTENSION ".eksc"
#define EK_SHADER_EXTENSION ".glsl"

namespace Eklipse 
{
    using AssetRegistry = std::map<AssetHandle, AssetMetadata>;

    class EK_API EditorAssetLibrary : public AssetLibrary
    {
    public:
        EditorAssetLibrary(const Path& assetDirectory);
        ~EditorAssetLibrary();

        virtual Ref<Asset> GetAsset(AssetHandle handle) override;
        virtual const AssetMetadata& GetMetadata(AssetHandle handle) const override;
        virtual bool IsAssetHandleValid(AssetHandle handle) const override;
        virtual bool IsAssetLoaded(AssetHandle handle) const override;
        virtual void UnloadAssets() override;
        virtual void LoadAssets() override;

        AssetRegistry& GetAssetRegistry();
        Path& GetAssetDirectory();
        AssetHandle ImportAsset(const Path& filepath);

        void SerializeAssetRegistry();
        bool DeserializeAssetRegistry();

        void OnFileWatchEvent(const String& path, filewatch::Event change_type);

        static AssetType GetAssetTypeFromFileExtension(const String& extension);
        static Vec<String> GetAssetFileExtensions(AssetType type);

        AssetHandle GetHandleFromAssetPath(const Path& path) const;

    private:
        AssetRegistry m_assetRegistry;
        Path m_assetDirectory;

        Unique<filewatch::FileWatch<String>> m_fileWatcher;
        bool m_shaderReloadPending;
    };
}