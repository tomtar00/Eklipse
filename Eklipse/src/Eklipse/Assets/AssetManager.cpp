#include "precompiled.h"
#include "AssetManager.h"

namespace Eklipse
{
    AssetLibrary* AssetManager::s_assetLibrary = nullptr;

    bool AssetManager::IsAssetHandleValid(AssetHandle handle)
    {
        return s_assetLibrary->IsAssetHandleValid(handle);
    }
    bool AssetManager::IsAssetLoaded(AssetHandle handle)
    {
        return s_assetLibrary->IsAssetLoaded(handle);
    }
    const AssetMetadata& AssetManager::GetMetadata(AssetHandle handle)
    {
		return s_assetLibrary->GetMetadata(handle);
    }
    const AssetMap& AssetManager::GetLoadedAssets()
    {
        return s_assetLibrary->GetLoadedAssets();
    }
}