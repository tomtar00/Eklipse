#pragma once
#include "VertexArray.h"
#include <Eklipse/Assets/AssetManager.h>

namespace Eklipse
{
	struct Triangle
	{
		glm::vec3 a;
		float pad0;
		glm::vec3 b;
		float pad1;
		glm::vec3 c;
		float pad2;
	};
	struct Bounds
	{
		glm::vec3 min;	float pad0;
		glm::vec3 max;  float pad1;
	};

	struct MeshData
	{
		BufferLayout layout;
		Vec<float> vertices;
		Vec<uint32_t> indices;
	};

	class EK_API Mesh : public Asset
	{
	public:
		Mesh() = delete;
		Mesh(const Path& filePath, const AssetHandle handle = AssetHandle());
		Mesh(const MeshData& data);
		static Ref<Mesh> Create(const Path& filePath, const AssetHandle handle = AssetHandle());
		static Ref<Mesh> Create(const MeshData& data);

		virtual void Dispose() override;

		const Ref<VertexArray>& GetVertexArray() const;
		const MeshData& GetMeshData() const;
		Vec<Triangle> GetTriangles() const;
		Vec<float> GetVertices() const;
		Vec<uint32_t> GetIndices() const;
		Bounds GetBounds() const;

		static AssetType GetStaticType() { return AssetType::Mesh; }
		virtual AssetType GetType() const override { return GetStaticType(); }

	private:
		Ref<VertexArray> m_vertexArray;
		MeshData m_meshData;
	};
}