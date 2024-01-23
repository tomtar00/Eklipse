#pragma once

#include "Asset.h"

#include <filesystem>

namespace Eklipse 
{
	struct AssetMetadata
	{
		AssetType Type = AssetType::None;
		std::filesystem::path FilePath;

		operator bool() const { return Type != AssetType::None; }
	};

}