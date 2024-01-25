#pragma once

#include "Asset.h"

#include <filesystem>

namespace Eklipse 
{
	struct AssetMetadata
	{
		AssetType Type = AssetType::None;
		Path FilePath;

		operator bool() const { return Type != AssetType::None; }
	};

}