#pragma once
#include <Eklipse/Renderer/Mesh.h>
#include <Eklipse/Renderer/Texture.h>
#include <Eklipse/Renderer/Shader.h>
#include <Eklipse/Renderer/Material.h>

namespace Eklipse
{
	enum class Operation
	{
		READ_ONLY,
		READ_WRITE,
	};

	class Assets
	{
	public:
		static void Init(const std::filesystem::path& assetsDirectoryPath);
		static void Shutdown();

		static Ref<Mesh>			GetMesh(const std::string& modelPath);
		static Ref<Texture2D>		GetTexture(const std::string& texturePath);
		static Ref<Shader>			GetShader(const std::string& shaderPath, Operation operation = Operation::READ_ONLY);
		static Ref<Material>		GetMaterial(const std::string& materialPath, Operation operation = Operation::READ_ONLY);

	private:
		static std::unordered_map<std::string, Ref<Mesh>, std::hash<std::string>>			s_meshCache;
		static std::unordered_map<std::string, Ref<Texture2D>, std::hash<std::string>>		s_textureCache;
		static std::unordered_map<std::string, Ref<Shader>, std::hash<std::string>>			s_shaderCache;
		static std::unordered_map<std::string, Ref<Material>, std::hash<std::string>>		s_materialCache;
	};
}