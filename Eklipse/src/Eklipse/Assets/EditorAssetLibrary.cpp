#include "precompiled.h"
#include "EditorAssetLibrary.h"
#include "AssetImporter.h"
#include "AssetManager.h"
#include "AssetMetadata.h"

#include <Eklipse/Utils/Yaml.h>
#include <Eklipse/Core/Application.h>

namespace Eklipse
{
    static std::map<Path, AssetType> s_assetExtensionMap = 
    {
        { EK_SCENE_EXTENSION,       AssetType::Scene        },
		{ EK_MATERIAL_EXTENSION,    AssetType::Material     },
		{ EK_SHADER_EXTENSION,      AssetType::Shader       },
        { ".png",                   AssetType::Texture2D    },
        { ".jpg",                   AssetType::Texture2D    },
        { ".jpeg",                  AssetType::Texture2D    },
        { ".obj", 				    AssetType::Mesh         }
    };

    static AssetType GetAssetTypeFromFileExtension(const Path& extension)
    {
        if (s_assetExtensionMap.find(extension) == s_assetExtensionMap.end())
        {
            EK_CORE_WARN("Could not find AssetType for {0}", extension);
            return AssetType::None;
        }

        return s_assetExtensionMap.at(extension);
    }

    EditorAssetLibrary::EditorAssetLibrary(const Path& assetDirectory) : m_assetDirectory(assetDirectory)
    {
        m_fileWatcher = CreateUnique<filewatch::FileWatch<String>>(assetDirectory.string(), CAPTURE_FN(OnFileWatchEvent));
    }

    Ref<Asset> EditorAssetLibrary::GetAsset(AssetHandle handle)
    {
        EK_CORE_TRACE("Getting asset with handle: {0}", handle);

        if (!IsAssetHandleValid(handle))
            return nullptr;

        Ref<Asset> asset;
        if (IsAssetLoaded(handle))
        {
            asset = m_loadedAssets.at(handle);
        }
        else
        {
            const AssetMetadata& metadata = GetMetadata(handle);
            asset = AssetImporter::ImportAsset(handle, metadata);
            if (!asset)
            {
                EK_CORE_ERROR("Failed to import asset with handle: {0}!", handle);
            }
            m_loadedAssets[handle] = asset;
        }
        return asset;
    }
    const AssetMetadata& EditorAssetLibrary::GetMetadata(AssetHandle handle) const
    {
        return m_assetRegistry.at(handle);
    }
    bool EditorAssetLibrary::IsAssetHandleValid(AssetHandle handle) const
    {
        return handle != 0 && m_assetRegistry.find(handle) != m_assetRegistry.end();
    }
    bool EditorAssetLibrary::IsAssetLoaded(AssetHandle handle) const
    {
        return m_loadedAssets.find(handle) != m_loadedAssets.end();
    }
    void EditorAssetLibrary::ImportAsset(const Path& filepath)
    {
        AssetHandle handle;
        AssetMetadata metadata;
        metadata.FilePath = filepath;
        metadata.Type = GetAssetTypeFromFileExtension(filepath.extension());
        EK_ASSERT(metadata.Type != AssetType::None, "Wrong asset type!");
        Ref<Asset> asset = AssetImporter::ImportAsset(handle, metadata);
        if (asset)
        {
            asset->Handle = handle;
            m_loadedAssets[handle] = asset;
            m_assetRegistry[handle] = metadata;
            SerializeAssetRegistry();
        }
    }
    
    void EditorAssetLibrary::SerializeAssetRegistry()
    {
        YAML::Emitter out;
        {
            out << YAML::BeginMap;
            out << YAML::Key << "AssetRegistry" << YAML::Value;

            out << YAML::BeginSeq;
            for (const auto& [handle, metadata] : m_assetRegistry)
            {
                out << YAML::BeginMap;
                out << YAML::Key << "Handle" << YAML::Value << handle;
                String filepathStr = metadata.FilePath.generic_string();
                out << YAML::Key << "FilePath" << YAML::Value << filepathStr;
                out << YAML::Key << "Type" << YAML::Value << AssetTypeToString(metadata.Type);
                out << YAML::EndMap;
            }
            out << YAML::EndSeq;
            out << YAML::EndMap;
        }

        std::ofstream fout(m_assetDirectory / "assets.ekreg");
        fout << out.c_str();
    }
    bool EditorAssetLibrary::DeserializeAssetRegistry()
    {
        auto path = (m_assetDirectory / "assets.ekreg").string();
        YAML::Node data;
        try
        {
            data = YAML::LoadFile(path);
        }
        catch (YAML::ParserException e)
        {
            EK_CORE_ERROR("Failed to load project file '{0}'. {1}", path, e.what());
            return false;
        }

        auto rootNode = data["AssetRegistry"];
        if (!rootNode)
            return false;

        for (const auto& node : rootNode)
        {
            AssetHandle handle = node["Handle"].as<uint64_t>();
            auto& metadata = m_assetRegistry[handle];
            metadata.FilePath = node["FilePath"].as<String>();
            metadata.Type = AssetTypeFromString(node["Type"].as<String>());
        }

        return true;
    }

    void EditorAssetLibrary::OnFileWatchEvent(const String& path, filewatch::Event change_type)
    {
        if (fs::is_directory(m_assetDirectory / path))
            return;

        switch (change_type)
        {
        case filewatch::Event::added:
            EK_CORE_TRACE("Asset added: {0}", path);
            break;
        case filewatch::Event::removed:
            EK_CORE_TRACE("Asset removed: {0}", path);
            break;
        case filewatch::Event::modified:
            EK_CORE_TRACE("Asset modified: {0}", path);
            {
                const String extension = Path(path).extension().string();
                const String pathString = (m_assetDirectory / path).string();

                if (extension == EK_SHADER_EXTENSION && !m_shaderReloadPending)
                {
                    AssetHandle shaderHandle = GetHandleFromAssetPath(path);
                    if (IsAssetHandleValid(shaderHandle))
                    {
                        m_shaderReloadPending = true;
                        Application::Get().SubmitToMainThread([&, pathString]()
                        {
                            Renderer::WaitDeviceIdle();

                            auto& shaderRef = AssetManager::GetAsset<Shader>(shaderHandle);

                            EK_CORE_DBG("Recompiling shader from path '{0}'", pathString);
                            if (!shaderRef->Recompile())
                            {
                                EK_CORE_ERROR("Failed to recompile shader at path '{0}'", pathString);
                            }

                            m_shaderReloadPending = false;
                        });
                    }
                    else
                    {
                        EK_CORE_TRACE("Shader at path '{0}' is not in the asset registry! Import this shader first.", pathString);
                    }
                }
            }
            break;
        default:
            EK_CORE_TRACE("Asset event: {0}", path);
            break;
        }
    }

    AssetHandle EditorAssetLibrary::GetHandleFromAssetPath(const Path& path) const
    {
        for (auto&& [handle, metadata] : m_assetRegistry)
        {
            if (metadata.FilePath == path)
                return handle;
        }
        return 0;
    }
}