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
		EditorAssetLibrary(const Path& assetDirectory);

		virtual Ref<Asset> GetAsset(AssetHandle handle) override;
		virtual const AssetMetadata& GetMetadata(AssetHandle handle) const override;
		virtual bool IsAssetHandleValid(AssetHandle handle) const override;
		virtual bool IsAssetLoaded(AssetHandle handle) const override;

		void ImportAsset(const Path& filepath);

		void SerializeAssetRegistry();
		bool DeserializeAssetRegistry();

		void OnFileWatchEvent(const String& path, filewatch::Event change_type);

	private:
		AssetHandle GetHandleFromAssetPath(const Path& path) const;

	private:
		AssetRegistry m_assetRegistry;
		Path m_assetDirectory;

		Unique<filewatch::FileWatch<String>> m_fileWatcher;
		bool m_shaderReloadPending;
	};
}