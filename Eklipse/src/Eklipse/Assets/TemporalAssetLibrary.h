#pragma once
#include "AssetLibrary.h"
#include "Asset.h"

namespace Eklipse
{
    using AssetRegistry = std::map<AssetHandle, AssetMetadata>;

    class TemporalAssetLibrary : public AssetLibrary
    {
    public:
        TemporalAssetLibrary();
        ~TemporalAssetLibrary();

        virtual Ref<Asset> GetAsset(AssetHandle handle) override;
        virtual AssetMetadata& GetMetadata(AssetHandle handle) override;
        virtual bool IsAssetHandleValid(AssetHandle handle) const override;
        virtual bool IsAssetLoaded(AssetHandle handle) const override;

        virtual AssetHandle ImportAsset(const Path& filepath) override;

        virtual void UnloadAssets() override;
        virtual void ReloadAssets() override;

    private:
        AssetRegistry m_assetRegistry;
        Path m_assetDirectory;
    };
}