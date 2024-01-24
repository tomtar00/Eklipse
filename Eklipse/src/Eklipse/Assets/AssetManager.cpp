#include "precompiled.h"
#include "AssetManager.h"

namespace Eklipse
{
    bool AssetManager::IsAssetHandleValid(AssetHandle handle)
    {
        return Application::Get().GetAssetLibrary()->IsAssetHandleValid(handle);
    }
    bool AssetManager::IsAssetLoaded(AssetHandle handle)
    {
        return Application::Get().GetAssetLibrary()->IsAssetLoaded(handle);
    }
    const AssetMetadata& AssetManager::GetMetadata(AssetHandle handle)
    {
		return Application::Get().GetAssetLibrary()->GetMetadata(handle);
    }
    const AssetMap& AssetManager::GetLoadedAssets()
    {
        return Application::Get().GetAssetLibrary()->GetLoadedAssets();
    }
}