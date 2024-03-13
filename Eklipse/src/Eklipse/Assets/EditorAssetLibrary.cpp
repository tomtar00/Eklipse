#include "precompiled.h"
#include "EditorAssetLibrary.h"
#include "AssetImporter.h"
#include "AssetManager.h"
#include "AssetMetadata.h"

#include <Eklipse/Utils/Yaml.h>
#include <Eklipse/Core/Application.h>
#include <Eklipse/Renderer/Material.h>

namespace Eklipse
{
    EditorAssetLibrary::EditorAssetLibrary(const Path& assetDirectory) 
        : m_shaderReloadPending(false), m_assetDirectory(assetDirectory)
    {
        EK_ASSERT(AssetManager::s_assetLibrary == nullptr, "AssetLibrary already exists!")
        Validate();
    }
    EditorAssetLibrary::~EditorAssetLibrary()
    {
        UnloadAssets();
    }

    Ref<Asset> EditorAssetLibrary::GetAsset(AssetHandle handle)
    {
        EK_CORE_PROFILE();
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
    AssetMetadata& EditorAssetLibrary::GetMetadata(AssetHandle handle)
    {
        EK_CORE_PROFILE();
        EK_ASSERT(IsAssetHandleValid(handle), "Invalid asset handle! ({})", handle);
        return m_assetRegistry.at(handle);
    }
    bool EditorAssetLibrary::IsAssetHandleValid(AssetHandle handle) const
    {
        EK_CORE_PROFILE();
        return handle != 0 && m_assetRegistry.find(handle) != m_assetRegistry.end();
    }
    bool EditorAssetLibrary::IsAssetLoaded(AssetHandle handle) const
    {
        EK_CORE_PROFILE();
        EK_ASSERT(IsAssetHandleValid(handle), "Invalid asset handle! ({})", handle);
        return m_loadedAssets.find(handle) != m_loadedAssets.end();
    }
    void EditorAssetLibrary::UnloadAssets()
    {
        EK_CORE_PROFILE();
        for (auto&& [handle, asset] : m_loadedAssets)
        {
            if (!asset)
                continue;

            asset->Dispose();
            asset.reset();
            asset = nullptr;
        }
    }
    void EditorAssetLibrary::ReloadAssets()
    {
        EK_CORE_PROFILE();
        Vec<AssetHandle> handlesToLoad;
        for (auto&& [handle, asset] : m_loadedAssets)
        {
            handlesToLoad.push_back(handle);
        }
        m_loadedAssets.clear();

        for (auto& handle : handlesToLoad)
        {
            GetAsset(handle);
        }
    }
    
    AssetRegistry& EditorAssetLibrary::GetAssetRegistry()
    {
        return m_assetRegistry;
    }
    Path& EditorAssetLibrary::GetAssetDirectory()
    {
       return m_assetDirectory;
    }
    AssetHandle EditorAssetLibrary::ImportAsset(const Path& filepath)
    {
        EK_CORE_PROFILE();
        EK_CORE_TRACE("Importing asset: {0}", filepath.string());

        AssetHandle handle;
        AssetMetadata metadata;
        metadata.FilePath = filepath;
        metadata.Type = Asset::GetTypeFromFileExtension(filepath.extension().string());
        if(metadata.Type == AssetType::None)
        {
            EK_CORE_ERROR("Failed to import asset from path: {0}. Unsupported asset type!", filepath.string());
            return 0;
        }
        Ref<Asset> asset = AssetImporter::ImportAsset(handle, metadata);
        if (asset)
        {
            asset->Handle = handle;
            m_loadedAssets[handle] = asset;
            m_assetRegistry[handle] = metadata;
            SerializeAssetRegistry();
        }

        EK_CORE_DBG("Asset from path '{0}' imported with handle: {1}", filepath.string(), handle);
        return handle;
    }
    AssetHandle EditorAssetLibrary::ImportDefaultMaterial(const Path& filepath, AssetHandle shaderHandle)
    {
        EK_CORE_PROFILE();
        AssetHandle handle;
        AssetMetadata metadata;
        metadata.FilePath = filepath;
        metadata.Type = Asset::GetTypeFromFileExtension(filepath.extension().string());
        EK_ASSERT(metadata.Type == AssetType::Material, "Wrong asset type!");
        Ref<Material> material = Material::Create(filepath, shaderHandle);
        if (material)
        {
            material->Handle = handle;
            m_loadedAssets[handle] = material;
            m_assetRegistry[handle] = metadata;
            SerializeAssetRegistry();
        }
        return handle;
    }
    void EditorAssetLibrary::RemoveAsset(AssetHandle handle)
    {
        EK_CORE_PROFILE();
        if (IsAssetHandleValid(handle))
        {
            EK_CORE_TRACE("Removing asset with handle: {0}", handle);

            if (IsAssetLoaded(handle))
            {
                auto& asset = m_loadedAssets.at(handle);
                if (asset)
                {
                    asset->Dispose();
                }
                m_loadedAssets.erase(handle);
            }
            m_assetRegistry.erase(handle);

            SerializeAssetRegistry();
        }
        else
        {
            EK_CORE_TRACE("Asset with handle '{0}' is not in the asset registry", handle);
        }
    }
    AssetHandle EditorAssetLibrary::GetHandleFromAssetPath(const Path& path, bool reqExists) const
    {
        EK_CORE_PROFILE();
        for (auto&& [handle, metadata] : m_assetRegistry)
        {
            if (reqExists && FileUtilities::ArePathsEqualAndExists(metadata.FilePath, path))
                return handle;
            else if (FileUtilities::ArePathsEqual(metadata.FilePath, path))
                return handle;
        }

        EK_CORE_WARN("Asset with path: {0} not found in the asset registry!", path.string());
        return 0;
    }
    void EditorAssetLibrary::Validate()
    {
        EK_CORE_PROFILE();
        EK_CORE_TRACE("Validating asset library...");

        bool hasInvalidAssets = false;
        for (auto&& [handle, metadata] : m_assetRegistry)
        {
            if (!FileUtilities::IsPathValid(metadata.FilePath))
            {
                hasInvalidAssets = true;
                EK_CORE_WARN("Asset with handle: {0} and path: {1} is not valid!", handle, metadata.FilePath.string());
                m_assetRegistry.erase(handle);
            }
        }

        if (hasInvalidAssets)
        {
            SerializeAssetRegistry();
        }

        EK_CORE_DBG("Asset library validated!");
    }
    
    bool EditorAssetLibrary::SerializeAssetRegistry(const AssetRegistry& registry, const Path& filepath)
    {
        EK_CORE_PROFILE();
        EK_CORE_TRACE("Serializing asset registry...");

        YAML::Emitter out;
        {
            out << YAML::BeginMap;
            out << YAML::Key << "AssetRegistry" << YAML::Value;

            out << YAML::BeginSeq;
            for (const auto& [handle, metadata] : registry)
            {
                out << YAML::BeginMap;
                out << YAML::Key << "Handle" << YAML::Value << handle;
                out << YAML::Key << "FilePath" << YAML::Value << metadata.FilePath.generic_string();
                out << YAML::Key << "Type" << YAML::Value << Asset::TypeToString(metadata.Type);
                out << YAML::EndMap;
            }
            out << YAML::EndSeq;
            out << YAML::EndMap;
        }

        std::ofstream fout(filepath);
        fout << out.c_str();

        EK_CORE_DBG("Asset registry serialized!");
        return true;
    }
    bool EditorAssetLibrary::DeserializeAssetRegistry(AssetRegistry& registry, const Path& filepath)
    {
        EK_CORE_PROFILE();
        EK_CORE_TRACE("Deserializing asset registry...");

        YAML::Node data;
        try
        {
            data = YAML::LoadFile(filepath.string());
        }
        catch (YAML::ParserException e)
        {
            EK_CORE_ERROR("Failed to load project file '{0}'. {1}", filepath.string(), e.what());
            return false;
        }

        auto rootNode = data["AssetRegistry"];
        if (!rootNode)
            return false;

        for (const auto& node : rootNode)
        {
            AssetHandle handle = TryDeserailize<AssetHandle>(node, "Handle", -1);
            auto& metadata = registry[handle];
            metadata.FilePath = TryDeserailize<String>(node, "FilePath", "");
            metadata.Type = Asset::TypeFromString(TryDeserailize<String>(node, "Type", ""));

            if (!FileUtilities::IsPathValid(metadata.FilePath))
            {
                EK_CORE_WARN("There no asset at path '{0}'", metadata.FilePath.string());
            }
        }

        EK_CORE_DBG("Asset registry deserialized!");
        return true;
    }
    bool EditorAssetLibrary::SerializeAssetRegistry()
    {
        EK_CORE_PROFILE();
        return SerializeAssetRegistry(m_assetRegistry, m_assetDirectory / ("assets" + String(EK_REGISTRY_EXTENSION)));
    }
    bool EditorAssetLibrary::DeserializeAssetRegistry()
    {
        EK_CORE_PROFILE();
        return DeserializeAssetRegistry(m_assetRegistry, m_assetDirectory / ("assets" + String(EK_REGISTRY_EXTENSION)));
    }

    void EditorAssetLibrary::StartFileWatcher()
    {
        m_fileWatcher = CreateUnique<filewatch::FileWatch<String>>(m_assetDirectory.string(), CAPTURE_FN(OnFileWatchEvent));
    }
    void EditorAssetLibrary::OnFileWatchEvent(const String& path, filewatch::Event change_type)
    {
        Path absolutePath = m_assetDirectory / path;
        if (fs::is_directory(absolutePath))
            return;

        switch (change_type)
        {
        case filewatch::Event::added:
            EK_CORE_TRACE("Asset added: {0}", path);
            {
                if (Path(path).extension() != EK_REGISTRY_EXTENSION)
                {
                    Application::Get().SubmitToMainThread([&, absolutePath]()
                    {
                        ImportAsset(absolutePath);
                    });
                }
            }
            break;
        case filewatch::Event::removed:
            EK_CORE_TRACE("Asset removed: {0}", path);
            {
                Application::Get().SubmitToMainThread([&, absolutePath]()
                {
                    AssetHandle handle = GetHandleFromAssetPath(absolutePath, false);
                    RemoveAsset(handle);
                });
            }
            break;
        case filewatch::Event::modified:
            EK_CORE_TRACE("Asset modified: {0}", path);
            {
                const String extension = absolutePath.extension().string();
                const String pathString = absolutePath.string();

                if (extension == EK_SHADER_EXTENSION && !m_shaderReloadPending)
                {
                    AssetHandle shaderHandle = GetHandleFromAssetPath(absolutePath);
                    if (IsAssetHandleValid(shaderHandle))
                    {
                        m_shaderReloadPending = true;
                        Application::Get().SubmitToMainThread([&, shaderHandle, absolutePath]()
                        {
                            Renderer::WaitDeviceIdle();

                            auto& shaderRef = AssetManager::GetAsset<Shader>(shaderHandle);

                            EK_CORE_DBG("Recompiling shader '{0}'", shaderHandle);
                            if (!shaderRef->Recompile(absolutePath))
                            {
                                EK_CORE_ERROR("Failed to recompile shader '{0}'", shaderHandle);
                            }

                            m_shaderReloadPending = false;
                        });
                    }
                    else
                    {
                        EK_CORE_TRACE("Shader at path '{0}' with handle '{1}' is not in the asset registry! Import this shader first.", pathString, shaderHandle);
                    }
                }
            }
            break;
        case filewatch::Event::renamed_old:
            EK_CORE_TRACE("Asset renamed_old: {0}", path);
            {
                Application::Get().SubmitToMainThread([&, absolutePath]()
                {
                    m_renamedAssetHandle = GetHandleFromAssetPath(absolutePath);
                    // TODO: what if the path is directory?
                });
            }
            break;
        case filewatch::Event::renamed_new:
            EK_CORE_TRACE("Asset renamed_new: {0}", path);
            {
                Application::Get().SubmitToMainThread([&, absolutePath]()
                {
                    if (IsAssetHandleValid(m_renamedAssetHandle))
                    {
                        Ref<Asset> asset = GetAsset(m_renamedAssetHandle);
                        if (asset)
                        {
                            asset->Name = absolutePath.stem().string();
                            GetMetadata(m_renamedAssetHandle).FilePath = absolutePath;
                            SerializeAssetRegistry();
                        }
                    }
                });
            }
            break;
        default:
            EK_CORE_TRACE("Asset event: {0}", path);
            break;
        }
    }
}