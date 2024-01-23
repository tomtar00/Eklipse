#include "precompiled.h"
#include "Asset.h"

namespace Eklipse
{
	const std::string& AssetTypeToString(AssetType type)
	{
		if (type == AssetType::None)			return "None";
		else if (type == AssetType::Scene)		return "Scene";
		else if (type == AssetType::Texture2D)	return "Texture2D";
		else if (type == AssetType::Material)	return "Material";
		else if (type == AssetType::Shader)		return "Shader";
		else if (type == AssetType::Mesh)		return "Mesh";
		else return "Unknown";
	}
	AssetType AssetTypeFromString(const std::string& assetType)
	{
		if (assetType == "None")			return AssetType::None;
		else if (assetType == "Scene")		return AssetType::Scene;
		else if (assetType == "Texture2D")	return AssetType::Texture2D;
		else if (assetType == "Material")	return AssetType::Material;
		else if (assetType == "Shader")		return AssetType::Shader;
		else if (assetType == "Mesh")		return AssetType::Mesh;
		else return AssetType::None;
	}
}