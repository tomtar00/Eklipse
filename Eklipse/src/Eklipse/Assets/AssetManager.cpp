#include "precompiled.h"
#include "AssetManager.h"

namespace Eklipse
{
    AssetLibrary* AssetManager::s_assetLibrary = nullptr;

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
    const AssetMap& AssetManager::GetLoadedAssets()
    {
        EK_CORE_PROFILE();
        return s_assetLibrary->GetLoadedAssets();
    }
}