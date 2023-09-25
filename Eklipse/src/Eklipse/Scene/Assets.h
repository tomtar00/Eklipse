#pragma once

namespace Eklipse
{
	struct MeshLoadResult
	{
		std::vector<float> vertices;
		std::vector<uint32_t> indices;
	};
	struct TextureLoadResult
	{
		int width;
		int height;
		uint32_t channels;
		unsigned char* data;
	};

	class AssetLoader
	{
	public:
		static MeshLoadResult LoadMesh(const char* modelPath);
		static TextureLoadResult LoadTexture(const char* texturePath);
	};
}