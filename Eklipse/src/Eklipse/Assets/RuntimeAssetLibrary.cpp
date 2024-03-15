#include "precompiled.h"
#include "RuntimeAssetLibrary.h"
#include "AssetManager.h"
#include "AssetImporter.h"

#define EK_REGISTRY_EXTENSION ".ekreg"

namespace Eklipse
{
    RuntimeAssetLibrary::RuntimeAssetLibrary(const Path& assetDirectory)
    {
        m_assetDirectory = assetDirectory;
        DeserializeAssetRegistry();
    }

    Ref<Asset> RuntimeAssetLibrary::GetAsset(AssetHandle handle)
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
    AssetMetadata& RuntimeAssetLibrary::GetMetadata(AssetHandle handle)
    {
        EK_CORE_PROFILE();
        EK_ASSERT(IsAssetHandleValid(handle), "Invalid asset handle! ({})", handle);
        return m_assetRegistry.at(handle);
    }
    bool RuntimeAssetLibrary::IsAssetHandleValid(AssetHandle handle) const
    {
        EK_CORE_PROFILE();
        return handle != 0 && m_assetRegistry.find(handle) != m_assetRegistry.end();
    }
    bool RuntimeAssetLibrary::IsAssetLoaded(AssetHandle handle) const
    {
        EK_CORE_PROFILE();
        EK_ASSERT(IsAssetHandleValid(handle), "Invalid asset handle! ({})", handle);
        return m_loadedAssets.find(handle) != m_loadedAssets.end();
    }

    AssetHandle RuntimeAssetLibrary::RegisterAsset(const Path& filepath)
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
    AssetHandle RuntimeAssetLibrary::ImportAsset(const Path& filepath)
    {
        EK_ASSERT(false, "Not implemented!");
        return AssetHandle();
    }

    void RuntimeAssetLibrary::UnloadAssets()
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
    void RuntimeAssetLibrary::ReloadAssets()
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

    bool RuntimeAssetLibrary::DeserializeAssetRegistry()
    {
        EK_CORE_PROFILE();
        EK_CORE_TRACE("Deserializing asset registry...");

        auto path = (m_assetDirectory / ("assets" + String(EK_REGISTRY_EXTENSION))).string();
        YAML::Node data;
        try
        {
            data = YAML::LoadFile(path);
        }
        catch (YAML::ParserException e)
        {
            EK_CORE_ERROR("Failed to load project file '{0}'. {1}", path, e.what());
            return false;
        }

        auto rootNode = data["AssetRegistry"];
        if (!rootNode)
            return false;

        for (const auto& node : rootNode)
        {
            AssetHandle handle = TryDeserailize<AssetHandle>(node, "Handle", -1);
            auto& metadata = m_assetRegistry[handle];
            metadata.FilePath = TryDeserailize<String>(node, "FilePath", "");
            metadata.Type = Asset::TypeFromString(TryDeserailize<String>(node, "Type", ""));
        }

        EK_CORE_DBG("Asset registry deserialized!");
        return true;
    }
}