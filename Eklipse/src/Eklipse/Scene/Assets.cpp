#include "precompiled.h"
#include "Assets.h"

#include <Eklipse/Renderer/Vertex.h>
#include <glm/gtx/hash.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace std
{
    template<> struct hash<Eklipse::Vertex>
    {
        size_t operator()(Eklipse::Vertex const& vertex) const
        {
            return ((hash<glm::vec3>()(vertex.pos) ^
                (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
                (hash<glm::vec2>()(vertex.texCoord) << 1);
        }
    };
}

namespace Eklipse
{
    std::unordered_map<std::string, Ref<Mesh>, std::hash<std::string>>          Assets::s_meshCache;
    std::unordered_map<std::string, Ref<Texture2D>, std::hash<std::string>>     Assets::s_textureCache;
    std::unordered_map<std::string, Ref<Shader>, std::hash<std::string>>        Assets::s_shaderCache;
    std::unordered_map<std::string, Ref<Material>, std::hash<std::string>>      Assets::s_materialCache;

    void Assets::Shutdown()
    {
        // Shaders
        for (auto&& [path, shader] : s_shaderCache)
        {
            shader->Dispose();
        }
        s_shaderCache.clear();
        
        // Textures
        for (auto&& [path, texture] : s_textureCache)
		{
			texture->Dispose();
		}
        s_textureCache.clear();

        // Meshes
        for (auto&& [path, mesh] : s_meshCache)
        {
            mesh->GetVertexArray()->Dispose();
        }
        s_meshCache.clear();
    }
    Ref<Mesh> Assets::GetMesh(const std::string& meshPath)
	{
        if (s_meshCache.find(meshPath) != s_meshCache.end())
        {
			return s_meshCache[meshPath];
		}   

        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, meshPath.c_str()))
        {
            EK_CORE_ERROR("Failed to load model at location: {0}. {1}", meshPath, warn + err);
        }

        std::unordered_map<Vertex, uint32_t, std::hash<Vertex>> uniqueVertices{};
        std::vector<float> vertices{};
        std::vector<uint32_t> indices{};

        for (const auto& shape : shapes)
        {
            for (const auto& index : shape.mesh.indices)
            {
                Vertex vertex{};

                vertex.pos =
                {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                };

                vertex.color = { 1.0f, 1.0f, 1.0f };

                if (attrib.texcoords.size() > 0)
                {
                    vertex.texCoord =
                    {
                        attrib.texcoords[2 * index.texcoord_index + 0],
                        1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                    };
                }

                if (uniqueVertices.count(vertex) == 0)
                {
                    uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size() / 8);
                    vertices.push_back(vertex.pos.x);
                    vertices.push_back(vertex.pos.y);
                    vertices.push_back(vertex.pos.z);
                    vertices.push_back(vertex.color.r);
                    vertices.push_back(vertex.color.g);
                    vertices.push_back(vertex.color.b);
                    vertices.push_back(vertex.texCoord.x);
                    vertices.push_back(vertex.texCoord.y);
                }

                indices.push_back(uniqueVertices[vertex]);
            }
        }

        Mesh mesh{ vertices, indices };
        Ref<Mesh> meshRef = CreateRef<Mesh>(mesh);
        s_meshCache[meshPath] = meshRef;

        EK_CORE_INFO("Loaded model from path {0}. Vertices: {1} Indices: {2}", meshPath, vertices.size() / 8, indices.size());
        return meshRef;
	}
    Ref<Texture2D> Assets::GetTexture(const std::string& texturePath)
    {
        if (s_textureCache.find(texturePath) != s_textureCache.end())
        {
			return s_textureCache[texturePath];
		}

        int width, height, channels;
        void* data = stbi_load(texturePath.c_str(), &width, &height, &channels, STBI_rgb_alpha);
        EK_ASSERT(data, "Failed to load texture image from location: {0}", texturePath);

        ImageFormat format = ImageFormat::FORMAT_UNDEFINED;
        if (channels == 3)
            format = ImageFormat::RGBA8; // ImageFormat::RGB8;
        else if (channels == 4)
            format = ImageFormat::RGBA8;

        TextureInfo textureInfo{};
        textureInfo.width = width;
        textureInfo.height = height;
        textureInfo.mipMapLevel = 1;
        textureInfo.samples = 1;
        textureInfo.imageFormat = format;
        textureInfo.imageAspect = ImageAspect::COLOR;
        textureInfo.imageUsage = ImageUsage::SAMPLED | ImageUsage::TRASNFER_DST;

        Ref<Texture2D> texture = Texture2D::Create(textureInfo);
        texture->SetData(data, width * height * 4/*channels*/);
        s_textureCache[texturePath] = texture;

        EK_CORE_INFO("Loaded texture from path '{0}'. Width: {1} Height: {2} Channels: {3}", texturePath, width, height, channels);
        return texture;
    }
    Ref<Shader> Assets::GetShader(const std::string& shaderPath)
    {
        if (s_shaderCache.find(shaderPath) != s_shaderCache.end())
        {
            return s_shaderCache[shaderPath];
        }

        Ref<Shader> shader = Shader::Create(shaderPath);
        s_shaderCache[shaderPath] = shader;
        EK_CORE_INFO("Loaded shader from path '{0}'", shaderPath);
        return shader;
    }
    Ref<Material> Assets::GetMaterial(const std::string& materialPath)
    {
        // TODO: insert return statement here
        EK_ASSERT(false, "Getting material from path not implemented!");

        if (s_materialCache.find(materialPath) != s_materialCache.end())
        {
            return s_materialCache[materialPath];
        }

        return Material::Create(s_shaderCache[0]);
    }
}
