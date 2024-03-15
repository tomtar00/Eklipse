#include "precompiled.h"
#include "Asset.h"

namespace Eklipse
{
	const String Asset::TypeToString(AssetType type)
	{
		EK_CORE_PROFILE();

		if (type == AssetType::None)				return "None";
		else if (type == AssetType::Scene)			return "Scene";
		else if (type == AssetType::Texture2D)		return "Texture2D";
		else if (type == AssetType::Material)		return "Material";
		else if (type == AssetType::Shader)			return "Shader";
		else if (type == AssetType::Mesh)			return "Mesh";
		else if (type == AssetType::ComputeShader)	return "ComputeShader";
		else return "Unknown";
	}
	AssetType Asset::TypeFromString(const String& assetType)
	{
		EK_CORE_PROFILE();

		if (assetType == "None")				return AssetType::None;
		else if (assetType == "Scene")			return AssetType::Scene;
		else if (assetType == "Texture2D")		return AssetType::Texture2D;
		else if (assetType == "Material")		return AssetType::Material;
		else if (assetType == "Shader")			return AssetType::Shader;
		else if (assetType == "Mesh")			return AssetType::Mesh;
		else if (assetType == "ComputeShader")	return AssetType::ComputeShader;
		else return AssetType::None;
	}

	AssetType Asset::GetTypeFromFileExtension(const String& extension)
	{
		EK_CORE_PROFILE();

		if (extension == EK_SCENE_EXTENSION)			return AssetType::Scene;
		if (extension == EK_MATERIAL_EXTENSION)			return AssetType::Material;
		if (extension == EK_SHADER_EXTENSION)			return AssetType::Shader;
		if (extension == EK_COMPUTE_SHADER_EXTENSION)	return AssetType::ComputeShader;

		if (extension == ".png")    return AssetType::Texture2D;
		if (extension == ".jpg")    return AssetType::Texture2D;
		if (extension == ".jpeg")   return AssetType::Texture2D;

		if (extension == ".obj") return AssetType::Mesh;

		return AssetType::None;
	}
	Vec<String> Asset::GetFileExtensions(AssetType type)
	{
		EK_CORE_PROFILE();

		if (type == AssetType::Scene)			return { EK_SCENE_EXTENSION };
		if (type == AssetType::Material)		return { EK_MATERIAL_EXTENSION };
		if (type == AssetType::Shader)			return { EK_SHADER_EXTENSION };
		if (type == AssetType::ComputeShader)	return { EK_COMPUTE_SHADER_EXTENSION };

		if (type == AssetType::Texture2D) return { ".png", ".jpg", ".jpeg" };
		if (type == AssetType::Mesh)      return { ".obj" };

		return {};
	}
}