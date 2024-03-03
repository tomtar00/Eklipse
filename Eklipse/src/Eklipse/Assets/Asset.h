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
		Mesh,
		ComputeShader
	};

	class EK_API Asset
	{
	public:
		static const String TypeToString(AssetType type);
		static AssetType TypeFromString(const String& assetType);

	public:
		AssetHandle Handle;
		String Name;

		virtual AssetType GetType() const = 0;
		virtual void Dispose() = 0;
	};
}