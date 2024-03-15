#pragma once

#include "AssetMetadata.h"

namespace Eklipse
{
	using AssetMap = std::map<AssetHandle, Ref<Asset>>;

	class AssetLibrary
	{
	public:
		virtual Ref<Asset> GetAsset(AssetHandle handle) = 0;
		virtual AssetMetadata& GetMetadata(AssetHandle handle) = 0;
		virtual bool IsAssetHandleValid(AssetHandle handle) const = 0;
		virtual bool IsAssetLoaded(AssetHandle handle) const = 0;

		virtual AssetHandle RegisterAsset(const Path& filepath) = 0;
		virtual AssetHandle ImportAsset(const Path& filepath) = 0;

		virtual void UnloadAssets() = 0;
		virtual void ReloadAssets() = 0;

		inline AssetMap& GetLoadedAssets() { return m_loadedAssets; }

	protected:
		AssetMap m_loadedAssets;
	};
}