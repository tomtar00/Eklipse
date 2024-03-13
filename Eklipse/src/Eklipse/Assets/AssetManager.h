#pragma once

#include <Eklipse/Core/Application.h>
#include "AssetMetadata.h"
#include "EditorAssetLibrary.h"
#include "RuntimeAssetLibrary.h"
#include "TemporalAssetLibrary.h"

namespace Eklipse 
{
	class EK_API AssetManager
	{
		friend EditorAssetLibrary;
		friend RuntimeAssetLibrary;
		friend TemporalAssetLibrary;

	public:

		static void SetLibrary(Ref<AssetLibrary> assetLibrary);

		template<typename T>
		static Ref<T> GetAsset(AssetHandle handle)
		{
			Ref<Asset> asset = s_assetLibrary->GetAsset(handle);
			return std::static_pointer_cast<T>(asset);
		}

		static bool IsAssetHandleValid(AssetHandle handle);
		static bool IsAssetHandleValidAndOfType(AssetHandle handle, AssetType type);
		static bool IsAssetLoaded(AssetHandle handle);
		static const AssetMetadata& GetMetadata(AssetHandle handle);

		static AssetHandle ImportAsset(const Path& filepath);

		static const AssetMap& GetLoadedAssets();

		static void UnloadAssets();
		static void ReloadAssets();

	private:
		static Ref<AssetLibrary> s_assetLibrary;
	};
}