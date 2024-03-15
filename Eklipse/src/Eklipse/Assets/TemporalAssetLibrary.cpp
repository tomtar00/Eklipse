#include "precompiled.h"
#include "TemporalAssetLibrary.h"
#include "AssetManager.h"
#include "AssetImporter.h"

#define EK_REGISTRY_EXTENSION ".ekreg"

namespace Eklipse
{
    TemporalAssetLibrary::TemporalAssetLibrary()
    {
    }
    TemporalAssetLibrary::~TemporalAssetLibrary()
    {
        UnloadAssets();
    }

    Ref<Asset> TemporalAssetLibrary::GetAsset(AssetHandle handle)
    {
        EK_CORE_PROFILE();
        EK_CORE_TRACE("Getting asset with handle: {0}", handle);

        if (!IsAssetHandleValid(handle))
            return nullptr;

        Ref<Asset> asset;
        if (IsAssetLoaded(handle))
        {
            asset = m_loadedAssets.at(handle);
        }
        else
        {
            const AssetMetadata& metadata = GetMetadata(handle);
            asset = AssetImporter::ImportAsset(handle, metadata);
            if (!asset)
            {
                EK_CORE_ERROR("Failed to import asset with handle: {0}!", handle);
            }
            m_loadedAssets[handle] = asset;
        }
        return asset;
    }
    AssetMetadata& TemporalAssetLibrary::GetMetadata(AssetHandle handle)
    {
        EK_CORE_PROFILE();
        EK_ASSERT(IsAssetHandleValid(handle), "Invalid asset handle! ({})", handle);
        return m_assetRegistry.at(handle);
    }
    bool TemporalAssetLibrary::IsAssetHandleValid(AssetHandle handle) const
    {
        EK_CORE_PROFILE();
        return handle != 0 && m_assetRegistry.find(handle) != m_assetRegistry.end();
    }
    bool TemporalAssetLibrary::IsAssetLoaded(AssetHandle handle) const
    {
        EK_CORE_PROFILE();
        EK_ASSERT(IsAssetHandleValid(handle), "Invalid asset handle! ({})", handle);
        return m_loadedAssets.find(handle) != m_loadedAssets.end();
    }

    AssetHandle TemporalAssetLibrary::RegisterAsset(const Path& filepath)
    {
        EK_CORE_PROFILE();
        EK_CORE_TRACE("Registring asset: {0}", filepath.string());

        AssetHandle handle;
        AssetMetadata metadata;
        metadata.FilePath = filepath;
        metadata.Type = Asset::GetTypeFromFileExtension(filepath.extension().string());
        if (metadata.Type == AssetType::None)
        {
            EK_CORE_ERROR("Failed to register asset from path: {0}. Unsupported asset type!", filepath.string());
            return 0;
        }
        bool assetValid = AssetImporter::ValidateAsset(handle, metadata);
        if (assetValid)
        {
            m_loadedAssets[handle] = nullptr;
            m_assetRegistry[handle] = metadata;
        }

        EK_CORE_DBG("Asset from path '{0}' registered with handle: {1}", filepath.string(), handle);
        return handle;
    }
    AssetHandle TemporalAssetLibrary::ImportAsset(const Path& filepath)
    {
        EK_CORE_PROFILE();
        EK_CORE_TRACE("Importing asset: {0}", filepath.string());

        AssetHandle handle;
        AssetMetadata metadata;
        metadata.FilePath = filepath;
        metadata.Type = Asset::GetTypeFromFileExtension(filepath.extension().string());
        if (metadata.Type == AssetType::None)
        {
            EK_CORE_ERROR("Failed to import asset from path: {0}. Unsupported asset type!", filepath.string());
            return 0;
        }
        Ref<Asset> asset = AssetImporter::ImportAsset(handle, metadata);
        if (asset)
        {
            asset->Handle = handle;
            m_loadedAssets[handle] = asset;
            m_assetRegistry[handle] = metadata;
        }

        EK_CORE_DBG("Asset from path '{0}' imported with handle: {1}", filepath.string(), handle);
        return handle;
    }

    void TemporalAssetLibrary::UnloadAssets()
    {
        EK_CORE_PROFILE();
        for (auto&& [handle, asset] : m_loadedAssets)
        {
            if (!asset)
                continue;

            asset->Dispose();
            asset.reset();
            asset = nullptr;
        }
    }
    void TemporalAssetLibrary::ReloadAssets()
    {
        EK_CORE_PROFILE();
        Vec<AssetHandle> handlesToLoad;
        for (auto&& [handle, asset] : m_loadedAssets)
        {
            handlesToLoad.push_back(handle);
        }
        m_loadedAssets.clear();

        for (auto& handle : handlesToLoad)
        {
            GetAsset(handle);
        }
    }
}