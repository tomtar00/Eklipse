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
        AssetManager::s_assetLibrary = this;

        DeserializeAssetRegistry();
    }
    Ref<Asset> RuntimeAssetLibrary::GetAsset(AssetHandle handle)
    {
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
    const AssetMetadata& RuntimeAssetLibrary::GetMetadata(AssetHandle handle) const
    {
        EK_ASSERT(IsAssetHandleValid(handle), "Invalid asset handle! ({})", handle);
        return m_assetRegistry.at(handle);
    }
    bool RuntimeAssetLibrary::IsAssetHandleValid(AssetHandle handle) const
    {
        return handle != 0 && m_assetRegistry.find(handle) != m_assetRegistry.end();
    }
    bool RuntimeAssetLibrary::IsAssetLoaded(AssetHandle handle) const
    {
        EK_ASSERT(IsAssetHandleValid(handle), "Invalid asset handle! ({})", handle);
        return m_loadedAssets.find(handle) != m_loadedAssets.end();
    }

    void RuntimeAssetLibrary::UnloadAssets()
    {
        for (auto&& [handle, asset] : m_loadedAssets)
        {
            asset->Dispose();
        }
    }
    void RuntimeAssetLibrary::ReloadAssets()
    {
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