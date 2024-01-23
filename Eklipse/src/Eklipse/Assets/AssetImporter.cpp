#include "precompiled.h"
#include "AssetImporter.h"

#include <Eklipse/Scene/Scene.h>
#include <Eklipse/Renderer/Texture.h>

namespace Eklipse
{
    static Ref<Scene> ImportScene(AssetHandle handle, const AssetMetadata& metadata)
    {
		return Scene::Load(metadata.FilePath);
	}
	static Ref<Texture2D> ImportTexture2D(AssetHandle handle, const AssetMetadata& metadata)
	{
		return Texture2D::Create(metadata.FilePath);
	}

    using AssetImportFunction = std::function<Ref<Asset>(AssetHandle, const AssetMetadata&)>;
    static std::map<AssetType, AssetImportFunction> s_assetImportFunctions = 
    {
        { AssetType::Texture2D,     ImportTexture2D },
        { AssetType::Scene,         ImportScene     }
    };

    Ref<Asset> AssetImporter::ImportAsset(AssetHandle handle, const AssetMetadata& metadata)
    {
        if (s_assetImportFunctions.find(metadata.Type) == s_assetImportFunctions.end())
        {
            EK_CORE_ERROR("No import function available for asset type: {}", AssetTypeToString(metadata.Type));
            return nullptr;
        }

        AssetImportFunction importFunction = s_assetImportFunctions.at(metadata.Type);
        return importFunction(handle, metadata);
    }
}