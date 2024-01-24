#include "precompiled.h"
#include "AssetImporter.h"

#include <Eklipse/Scene/Scene.h>
#include <Eklipse/Renderer/Texture.h>
#include <Eklipse/Renderer/Mesh.h>
#include <Eklipse/Renderer/Shader.h>
#include <Eklipse/Renderer/Material.h>

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
    static Ref<Mesh> ImportMesh(AssetHandle handle, const AssetMetadata& metadata)
    {
        return Mesh::Create(metadata.FilePath);
    }
    static Ref<Shader> ImportShader(AssetHandle handle, const AssetMetadata& metadata)
    {
		return Shader::Create(metadata.FilePath);
	}
    static Ref<Material> ImportMaterial(AssetHandle handle, const AssetMetadata& metadata)
    {
		return Material::Create(metadata.FilePath);
	}

    using AssetImportFunction = std::function<Ref<Asset>(AssetHandle, const AssetMetadata&)>;
    static std::map<AssetType, AssetImportFunction> s_assetImportFunctions = 
    {
        { AssetType::Texture2D,     ImportTexture2D },
        { AssetType::Scene,         ImportScene     },
		{ AssetType::Mesh,          ImportMesh      },
		{ AssetType::Shader,        ImportShader    },
		{ AssetType::Material,      ImportMaterial  }
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