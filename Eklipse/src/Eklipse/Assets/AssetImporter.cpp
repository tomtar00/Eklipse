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
        EK_CORE_PROFILE();
		return Scene::Load(metadata.FilePath, handle);
	}
	static Ref<Texture2D> ImportTexture2D(AssetHandle handle, const AssetMetadata& metadata)
	{
        EK_CORE_PROFILE();
		return Texture2D::Create(metadata.FilePath, handle);
	}
    static Ref<Mesh> ImportMesh(AssetHandle handle, const AssetMetadata& metadata)
    {
        EK_CORE_PROFILE();
        return Mesh::Create(metadata.FilePath, handle);
    }
    static Ref<Shader> ImportShader(AssetHandle handle, const AssetMetadata& metadata)
    {
        EK_CORE_PROFILE();
		return Shader::Create(metadata.FilePath, handle);
	}
    static Ref<Material> ImportMaterial(AssetHandle handle, const AssetMetadata& metadata)
    {
        EK_CORE_PROFILE();
		auto mat = Material::Create(metadata.FilePath, 0);
        mat->Handle = handle;
        return mat;
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
        EK_CORE_PROFILE();
        if (s_assetImportFunctions.find(metadata.Type) == s_assetImportFunctions.end())
        {
            EK_CORE_ERROR("No import function available for asset type: {}", Asset::TypeToString(metadata.Type));
            return nullptr;
        }

        AssetImportFunction importFunction = s_assetImportFunctions.at(metadata.Type);
        Ref<Asset> asset = importFunction(handle, metadata);
        return asset;
    }
}