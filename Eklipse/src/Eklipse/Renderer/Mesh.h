#pragma once
#include "VertexArray.h"

namespace Eklipse
{
	class EK_API Mesh
	{
	public:
		Mesh() = delete;
		Mesh(std::vector<float> vertices, std::vector<uint32_t> indices, const std::string& path);

		inline const Ref<VertexArray>& GetVertexArray() const { return m_vertexArray; }
		inline const std::string& GetPath() const { return m_path; }

	private:
		Ref<VertexArray> m_vertexArray;
		std::string m_path;
	};
}