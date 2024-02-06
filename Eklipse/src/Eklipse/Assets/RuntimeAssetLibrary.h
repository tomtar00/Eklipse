#pragma once
#include "AssetLibrary.h"
#include "Asset.h"

// TODO: This is a temporary implementation of the runtime asset library. 
// It will be replaced with a more robust system in the future.

namespace Eklipse
{
    using AssetRegistry = std::map<AssetHandle, AssetMetadata>;

    class EK_API RuntimeAssetLibrary : public AssetLibrary
    {
    public:
        RuntimeAssetLibrary(const Path& assetDirectory);

        virtual Ref<Asset> GetAsset(AssetHandle handle) override;
        virtual const AssetMetadata& GetMetadata(AssetHandle handle) const override;
        virtual bool IsAssetHandleValid(AssetHandle handle) const override;
        virtual bool IsAssetLoaded(AssetHandle handle) const override;

        virtual void UnloadAssets() override;
        virtual void ReloadAssets() override;

    private:
        bool DeserializeAssetRegistry();

    private:
        AssetRegistry m_assetRegistry;
        Path m_assetDirectory;
    };
}