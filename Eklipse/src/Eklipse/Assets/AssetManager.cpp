#include "precompiled.h"
#include "AssetManager.h"

namespace Eklipse
{
    Ref<AssetLibrary> AssetManager::s_assetLibrary = nullptr;

    void AssetManager::SetLibrary(Ref<AssetLibrary> assetLibrary)
    {
        EK_CORE_PROFILE();
        s_assetLibrary = assetLibrary;
    }

    bool AssetManager::IsAssetHandleValid(AssetHandle handle)
    {
        EK_CORE_PROFILE();
        return s_assetLibrary->IsAssetHandleValid(handle);
    }
    bool AssetManager::IsAssetHandleValidAndOfType(AssetHandle handle, AssetType type)
    {
        EK_CORE_PROFILE();
        return IsAssetHandleValid(handle) && GetMetadata(handle).Type == type;
    }
    bool AssetManager::IsAssetLoaded(AssetHandle handle)
    {
        EK_CORE_PROFILE();
        return s_assetLibrary->IsAssetLoaded(handle);
    }
    const AssetMetadata& AssetManager::GetMetadata(AssetHandle handle)
    {
        EK_CORE_PROFILE();
		return s_assetLibrary->GetMetadata(handle);
    }
    AssetHandle AssetManager::ImportAsset(const Path& filepath)
    {
        EK_CORE_PROFILE();
        return s_assetLibrary->ImportAsset(filepath);
    }
    const AssetMap& AssetManager::GetLoadedAssets()
    {
        EK_CORE_PROFILE();
        return s_assetLibrary->GetLoadedAssets();
    }
    void AssetManager::UnloadAssets()
    {
        EK_CORE_PROFILE();
        s_assetLibrary->UnloadAssets();
    }
    void AssetManager::ReloadAssets()
    {
        EK_CORE_PROFILE();
        s_assetLibrary->ReloadAssets();
    }
}