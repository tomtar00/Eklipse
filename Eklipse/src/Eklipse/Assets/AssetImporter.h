#pragma once

#include "AssetMetadata.h"

namespace Eklipse 
{
	class AssetImporter
	{
	public:
		static Ref<Asset> ImportAsset(AssetHandle handle, const AssetMetadata& metadata);
		static bool ValidateAsset(AssetHandle handle, const AssetMetadata& metadata);
	};

}