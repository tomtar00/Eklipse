#include "precompiled.h"
//#include "AssetLibrary.h"
//
//#include <Eklipse/Renderer/Vertex.h>
//#include <glm/gtx/hash.hpp>
//
//#define TINYOBJLOADER_IMPLEMENTATION
//#include <tiny_obj_loader.h>
//
//#include <Eklipse/Utils/File.h>
//#include <Eklipse/Core/Application.h>
//
//namespace std
//{
//    template<> struct hash<Eklipse::Vertex>
//    {
//        size_t operator()(Eklipse::Vertex const& vertex) const
//        {
//            return ((hash<glm::vec3>()(vertex.pos) ^
//                (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
//                (hash<glm::vec2>()(vertex.texCoord) << 1);
//        }
//    };
//}
//
//namespace Eklipse
//{
//    void AssetLibrary::Load(const Path& assetsDirectoryPath)
//    {
//        // Load all recognized assets
//        m_assetsDirectoryPath = assetsDirectoryPath;
//        if (!std::filesystem::exists(assetsDirectoryPath.path()))
//		{
//			std::filesystem::create_directories(assetsDirectoryPath.path());
//		}
//        for (const auto& directoryEntry : std::filesystem::recursive_directory_iterator(assetsDirectoryPath))
//        {
//            if (std::filesystem::is_directory(directoryEntry.path()))
//                continue;
//
//            Path relativePath = std::filesystem::relative(directoryEntry.path());
//            if (relativePath.empty())
//				relativePath = directoryEntry.path();
//            const std::string extension = relativePath.path().extension().string();
//            const std::string pathString = relativePath.full_string();
//
//            if (extension == ".obj")
//            {
//                EK_CORE_DBG("Loading model from path full='{0}' relative='{1}'", relativePath.full_string(), relativePath.string());
//                GetMesh(relativePath);
//            }
//            else if (extension == ".png" || extension == ".jpg" || extension == ".jpeg" || extension == ".bmp")
//            {
//                EK_CORE_DBG("Loading texture from path full='{0}' relative='{1}'", relativePath.full_string(), relativePath.string());
//                GetTexture(relativePath);
//            }
//            else if (extension == ".eksh")
//            {
//                EK_CORE_DBG("Loading shader from path full='{0}' relative='{1}'", relativePath.full_string(), relativePath.string());
//                GetShader(relativePath);
//            }
//            else if (extension == ".ekmt")
//            {
//                EK_CORE_DBG("Loading material from path full='{0}' relative='{1}'", relativePath.full_string(), relativePath.string());
//                GetMaterial(relativePath);
//            }
//        }
//        EK_CORE_DBG("Loaded {0} models, {1} textures, {2} shaders and {3} materials", m_meshCache.size(), m_textureCache.size(), m_shaderCache.size(), m_materialCache.size());
//
//        // Monitor assets changes with FileWatch
//        m_fileWatcher = CreateUnique<filewatch::FileWatch<std::string>>(assetsDirectoryPath.full_string(), CAPTURE_FN(OnFileWatchEvent));
//        EK_CORE_DBG("Monitoring assets directory '{0}'", assetsDirectoryPath.string());
//    }
//    void AssetLibrary::Unload()
//    {
//        // Shaders
//        for (auto&& [path, shader] : m_shaderCache)
//        {
//            shader->Dispose();
//        }
//        m_shaderCache.clear();
//
//        // Textures
//        for (auto&& [path, texture] : m_textureCache)
//        {
//            texture->Dispose();
//        }
//        m_textureCache.clear();
//
//        // Meshes
//        for (auto&& [path, mesh] : m_meshCache)
//        {
//            mesh->GetVertexArray()->Dispose();
//        }
//        m_meshCache.clear();
//
//        // Materials
//        for (auto&& [path, material] : m_materialCache)
//        {
//            material->Dispose();
//        }
//        m_materialCache.clear();
//    }
//
//    Ref<Mesh> AssetLibrary::GetMesh(const Path& meshPath)
//    {
//        auto it = m_meshCache.find(meshPath);
//        if (it != m_meshCache.end())
//        {
//            return it->second;
//        }
//
//        tinyobj::attrib_t attrib;
//        std::vector<tinyobj::shape_t> shapes;
//        std::vector<tinyobj::material_t> materials;
//        std::string warn, err;
//
//        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, meshPath.full_c_str()))
//        {
//            EK_CORE_ERROR("Failed to load model at location: {0}. {1}", meshPath, warn + err);
//            return nullptr;
//        }
//
//        std::unordered_map<Vertex, uint32_t, std::hash<Vertex>> uniqueVertices{};
//        std::vector<float> vertices{};
//        std::vector<uint32_t> indices{};
//
//        for (const auto& shape : shapes)
//        {
//            for (const auto& index : shape.mesh.indices)
//            {
//                Vertex vertex{};
//
//                vertex.pos =
//                {
//                    attrib.vertices[3 * index.vertex_index + 0],
//                    attrib.vertices[3 * index.vertex_index + 1],
//                    attrib.vertices[3 * index.vertex_index + 2]
//                };
//
//                vertex.color = { 1.0f, 1.0f, 1.0f };
//
//                if (attrib.texcoords.size() > 0)
//                {
//                    vertex.texCoord =
//                    {
//                        attrib.texcoords[2 * index.texcoord_index + 0],
//                        1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
//                    };
//                }
//
//                if (uniqueVertices.count(vertex) == 0)
//                {
//                    uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size() / 8);
//                    vertices.push_back(vertex.pos.x);
//                    vertices.push_back(vertex.pos.y);
//                    vertices.push_back(vertex.pos.z);
//                    vertices.push_back(vertex.color.r);
//                    vertices.push_back(vertex.color.g);
//                    vertices.push_back(vertex.color.b);
//                    vertices.push_back(vertex.texCoord.x);
//                    vertices.push_back(vertex.texCoord.y);
//                }
//
//                indices.push_back(uniqueVertices[vertex]);
//            }
//        }
//
//        Mesh mesh{ vertices, indices, meshPath };
//        Ref<Mesh> meshRef = CreateRef<Mesh>(mesh);
//        m_meshCache[meshPath] = meshRef;
//
//        EK_CORE_DBG("Loaded model from path {0}. Vertices: {1} Indices: {2}", meshPath, vertices.size() / 8, indices.size());
//        return meshRef;
//    }
//    Ref<Texture2D> AssetLibrary::GetTexture(const Path& texturePath)
//    {
//        auto it = m_textureCache.find(texturePath);
//        if (it != m_textureCache.end())
//        {
//            return it->second;
//        }
//
//        int width, height, channels;
//        void* data = stbi_load(texturePath.full_c_str(), &width, &height, &channels, STBI_rgb_alpha);
//
//        if (data == nullptr)
//        {
//            EK_CORE_ERROR("Failed to load texture from location: {0}", texturePath);
//            return nullptr;
//        }
//
//        ImageFormat format = ImageFormat::FORMAT_UNDEFINED;
//        if (channels == 3)
//            format = ImageFormat::RGBA8; // ImageFormat::RGB8;
//        else if (channels == 4)
//            format = ImageFormat::RGBA8;
//
//        TextureInfo textureInfo{};
//        textureInfo.width = width;
//        textureInfo.height = height;
//        textureInfo.mipMapLevel = 1;
//        textureInfo.samples = 1;
//        textureInfo.imageFormat = format;
//        textureInfo.imageAspect = ImageAspect::COLOR;
//        textureInfo.imageUsage = ImageUsage::SAMPLED | ImageUsage::TRASNFER_DST;
//
//        Ref<Texture2D> texture = Texture2D::Create(textureInfo, texturePath);
//        texture->SetData(data, width * height * 4/*channels*/);
//        m_textureCache[texturePath] = texture;
//
//        EK_CORE_DBG("Loaded texture from path '{0}'. Width: {1} Height: {2} Channels: {3}", texturePath, width, height, channels);
//        return texture;
//    }
//    Ref<Shader> AssetLibrary::GetShader(const Path& shaderPath)
//    {
//        auto it = m_shaderCache.find(shaderPath);
//        if (it != m_shaderCache.end())
//        {
//            return it->second;
//        }
//
//        Ref<Shader> shader = Shader::Create(shaderPath);
//        m_shaderCache[shaderPath] = shader;
//
//        EK_CORE_DBG("Loaded shader from path '{0}'", shaderPath);
//        return shader;
//    }
//    Ref<Material> AssetLibrary::GetMaterial(const Path& materialPath, const Path& shaderPath)
//    {
//        auto it = m_materialCache.find(materialPath);
//        if (it != m_materialCache.end())
//        {
//            return it->second;
//        }
//
//        auto& material = Material::Create(materialPath, shaderPath);
//        m_materialCache[materialPath] = material;
//
//        EK_CORE_DBG("Loaded material from path '{0}'", materialPath);
//        return material;
//    }
//    void AssetLibrary::OnFileWatchEvent(const std::string& path, filewatch::Event change_type)
//    {
//        if (std::filesystem::is_directory(m_assetsDirectoryPath.path() / path))
//			return;
//
//        switch (change_type)
//        {
//            case filewatch::Event::added:
//                EK_CORE_TRACE("File added: {0}", path);
//                break;
//            case filewatch::Event::removed:
//                EK_CORE_TRACE("File removed: {0}", path);
//                break;
//            case filewatch::Event::modified:
//                EK_CORE_TRACE("File modified: {0}", path);
//                {
//                    const std::string extension = std::filesystem::path(path).extension().string();
//					const std::string pathString = Path(m_assetsDirectoryPath.path() / path).string();
//
//                    if (!m_shaderReloadPending && extension == EK_SHADER_EXTENSION)
//                    {
//                        // if shader exists in chache == if is not being created now (makes sure this compilation doesnt run on shader creation)
//                        if (m_shaderCache.find(pathString) != m_shaderCache.end())
//                        {
//                            m_shaderReloadPending = true;
//                            Application::Get().SubmitToMainThread([&, pathString]()
//                            {
//                                Eklipse::Renderer::WaitDeviceIdle();
//
//                                auto& shaderRef = GetShader(pathString);
//
//                                EK_CORE_DBG("Recompiling shader from path '{0}'", pathString);
//                                bool recompiledSuccessfully = shaderRef->Recompile();
//                                if (recompiledSuccessfully)
//                                {
//                                    for (auto&& [path, material] : m_materialCache)
//                                    {
//                                        if (material->GetShader() == shaderRef)
//                                        {
//                                            EK_CORE_TRACE("Reloading material at path '{0}', because shader at path '{1}' has been recompiled", material->GetPath().string(), shaderRef->GetPath().string());
//                                            material->OnShaderReloaded();
//                                            material->ApplyChanges();
//                                        }
//                                    }
//                                }
//                                else
//                                {
//                                    EK_CORE_ERROR("Failed to recompile shader at path '{0}'", pathString);
//                                }
//
//                                m_shaderReloadPending = false;
//                            });
//                        }  
//                        else
//                        {
//                            EK_CORE_TRACE("Shader at path '{0}' is not in cache, so it will not be recompiled", pathString);
//                        }
//					}
//                }
//                break;
//            default:
//                EK_CORE_TRACE("File event: {0}", path);
//                break;
//        }
//    }
//}