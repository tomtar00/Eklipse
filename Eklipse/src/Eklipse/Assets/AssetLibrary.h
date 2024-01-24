#pragma once

#include "Asset.h"

namespace Eklipse
{
	using AssetMap = std::map<AssetHandle, Ref<Asset>>;

	class AssetLibrary
	{
	public:
		virtual Ref<Asset> GetAsset(AssetHandle handle) = 0;
		virtual const AssetMetadata& GetMetadata(AssetHandle handle) const = 0;
		virtual bool IsAssetHandleValid(AssetHandle handle) const = 0;
		virtual bool IsAssetLoaded(AssetHandle handle) const = 0;

		inline AssetMap& GetLoadedAssets() { return m_loadedAssets; }

	protected:
		AssetMap m_loadedAssets;
	};
}