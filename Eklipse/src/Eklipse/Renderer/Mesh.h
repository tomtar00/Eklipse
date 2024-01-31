#pragma once
#include "VertexArray.h"
#include <Eklipse/Assets/AssetManager.h>

namespace Eklipse
{
	struct MeshData
	{
		BufferLayout layout;
		Vec<float> vertices;
		Vec<uint32_t> indices;
	};

	class Mesh : public Asset
	{
	public:
		Mesh() = delete;
		Mesh(const Path& filePath, const AssetHandle handle = AssetHandle());
		Mesh(const MeshData& data);
		static Ref<Mesh> Create(const Path& filePath, const AssetHandle handle = AssetHandle());
		static Ref<Mesh> Create(const MeshData& data);

		const Ref<VertexArray>& GetVertexArray() const;

		static AssetType GetStaticType() { return AssetType::Mesh; }
		virtual AssetType GetType() const override { return GetStaticType(); }

	private:
		Ref<VertexArray> m_vertexArray;
	};
}