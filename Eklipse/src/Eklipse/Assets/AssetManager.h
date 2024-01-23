#pragma once

#include <Eklipse/Core/Application.h>

namespace Eklipse 
{
	class AssetManager
	{
	public:
		template<typename T>
		static Ref<T> GetAsset(AssetHandle handle)
		{
			Ref<Asset> asset = Application::Get().GetAssetLibrary()->GetAsset(handle);
			return std::static_pointer_cast<T>(asset);
		}

		static bool IsAssetHandleValid(AssetHandle handle);
		static bool IsAssetLoaded(AssetHandle handle);
		static const AssetMetadata& GetMetadata(AssetHandle handle);
	};
}