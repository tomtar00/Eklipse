#pragma once

#include <Eklipse/Core/UUID.h>

namespace Eklipse {

	using AssetHandle = UUID;

	enum class AssetType : uint16_t
	{
		None = 0,
		Scene,
		Texture2D,
		Material,
		Shader,
		Mesh
	};

	const String& AssetTypeToString(AssetType type);
	AssetType AssetTypeFromString(const String& assetType);

	class Asset
	{
	public:
		AssetHandle Handle;

		virtual AssetType GetType() const = 0;
	};
}