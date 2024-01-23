#pragma once

#include "AssetLibrary.h"
#include "AssetMetadata.h"

#include <FileWatch.hpp>

#define EK_MATERIAL_EXTENSION ".ekmt"
#define EK_SCENE_EXTENSION ".eksc"
#define EK_SHADER_EXTENSION ".glsl"

namespace Eklipse 
{
	using AssetRegistry = std::map<AssetHandle, AssetMetadata>;

	class EditorAssetLibrary : public AssetLibrary
	{
	public:
		EditorAssetLibrary(const std::filesystem::path& assetDirectory);

		virtual Ref<Asset> GetAsset(AssetHandle handle) override;
		virtual const AssetType& GetType(AssetHandle handle) const override;
		virtual bool IsAssetHandleValid(AssetHandle handle) const override;
		virtual bool IsAssetLoaded(AssetHandle handle) const override;

		void ImportAsset(const std::filesystem::path& filepath);

		void SerializeAssetRegistry();
		bool DeserializeAssetRegistry();

		void OnFileWatchEvent(const std::string& path, filewatch::Event change_type);

	private:
		AssetHandle GetHandleFromAssetPath(const std::filesystem::path& path) const;

	private:
		AssetRegistry m_assetRegistry;
		AssetMap m_loadedAssets;
		std::filesystem::path m_assetDirectory;

		Unique<filewatch::FileWatch<std::string>> m_fileWatcher;
		bool m_shaderReloadPending;
	};
}