#pragma once

#include <Eklipse/Core/Application.h>
#include "AssetMetadata.h"
#include "EditorAssetLibrary.h"
#include "RuntimeAssetLibrary.h"

namespace Eklipse 
{
	class EK_API AssetManager
	{
		friend EditorAssetLibrary;
		friend RuntimeAssetLibrary;

	public:
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

		static const AssetMap& GetLoadedAssets();

	private:
		static AssetLibrary* s_assetLibrary;
	};
}