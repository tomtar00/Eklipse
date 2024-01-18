#pragma once
#include <Eklipse/Renderer/Mesh.h>
#include <Eklipse/Renderer/Texture.h>
#include <Eklipse/Renderer/Shader.h>
#include <Eklipse/Renderer/Material.h>
#include <Eklipse/Utils/File.h>

#define EK_MATERIAL_EXTENSION ".ekmt"
#define EK_SHADER_EXTENSION ".eksh"

#include <FileWatch.hpp>

namespace Eklipse
{
	class EK_API AssetLibrary
	{
	public:
		AssetLibrary() = default;
		void Load(const Path& assetsDirectoryPath);
		void Unload();

		inline const std::unordered_map<std::string, Ref<Mesh>, std::hash<std::string>>& GetMeshCache()			{ return m_meshCache; }
		inline const std::unordered_map<std::string, Ref<Texture2D>, std::hash<std::string>>& GetTextureCache() { return m_textureCache; }
		inline const std::unordered_map<std::string, Ref<Shader>, std::hash<std::string>>& GetShaderCache()		{ return m_shaderCache; }
		inline const std::unordered_map<std::string, Ref<Material>, std::hash<std::string>>& GetMaterialCache() { return m_materialCache; }

		Ref<Mesh>		GetMesh(const Path& modelPath);
		Ref<Texture2D>	GetTexture(const Path& texturePath);
		Ref<Shader>		GetShader(const Path& shaderPath);
		Ref<Material>	GetMaterial(const Path& materialPath, const Path& shaderPath = "");

		const Path& GetAssetsDirectoryPath() const { return m_assetsDirectoryPath; }

	private:
		void OnFileWatchEvent(const std::string& path, filewatch::Event change_type);

	private:
		std::unordered_map<std::string, Ref<Mesh>, std::hash<std::string>>			m_meshCache;
		std::unordered_map<std::string, Ref<Texture2D>, std::hash<std::string>>		m_textureCache;
		std::unordered_map<std::string, Ref<Shader>, std::hash<std::string>>		m_shaderCache;
		std::unordered_map<std::string, Ref<Material>, std::hash<std::string>>		m_materialCache;

		Path m_assetsDirectoryPath;
		Unique<filewatch::FileWatch<std::string>> m_fileWatcher;

		bool m_shaderReloadPending = false;
	};
}