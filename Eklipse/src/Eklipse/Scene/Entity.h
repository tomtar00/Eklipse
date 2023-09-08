#pragma once

#include "Transform.h"
#include "Components.h"
#include <Eklipse/Renderer/UBO.h>
#include <Eklipse/Renderer/VertexArray.h>

namespace Eklipse
{
	class Entity
	{
	public:
		Entity(const std::string& name, Mesh& mesh);

		void UpdateModelMatrix(glm::mat4 viewProjMatrix);

	private:
		inline static int s_idCounter = 0;
		
	public:
		uint32_t m_id;
		std::string m_name;

		Transform m_transform;
		ModelViewProjUBO m_ubo;
		Ref<VertexArray> m_vertexArray;
		Ref<UniformBuffer> m_uniformBuffer;
		Ref<Texture> m_texture;

		//std::vector<Component> m_components;
		Mesh m_mesh;
	};
}