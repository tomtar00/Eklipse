#pragma once

#include "Transform.h"
#include "Components.h"
#include <Eklipse/Renderer/UBO.h>

namespace Eklipse
{
	class Entity
	{
	public:
		Entity(std::string name);

		void UpdateModelMatrix(glm::mat4 viewProjMatrix);

		//std::vector<Component> m_components;
		Mesh m_mesh;

		Transform m_transform;
		ModelViewProjUBO m_ubo;

		std::string m_name;
		uint32_t m_id;
	private:
		static int s_idCounter;
	};
}