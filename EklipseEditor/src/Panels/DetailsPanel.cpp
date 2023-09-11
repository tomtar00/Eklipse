#include "DetailsPanel.h"
#include "EntitiesPanel.h"
#include <misc/cpp/imgui_stdlib.h>
#include <glm/gtc/type_ptr.hpp>

namespace Editor
{
	void DetailsPanel::Setup(Eklipse::Entity* entity)
	{
		m_entity = entity;
		m_entityNameBuffer = m_entity->m_name;
	}
	void DetailsPanel::OnGUI()
	{
		ImGui::Begin("Details");
		if (m_entity == nullptr)
		{
			ImGui::End(); 
			return;
		}

		if (!ImGui::InputText("Entity Name", &m_entityNameBuffer))
		{
			if (m_entityNameBuffer.size() > 0)
			{
				m_entity->m_name = m_entityNameBuffer;
			}
		}

		ImGui::InputFloat3("Position", glm::value_ptr(m_entity->m_transform.position), "%.5f");
		ImGui::InputFloat3("Rotation", glm::value_ptr(m_entity->m_transform.rotation), "%.5f");
		ImGui::InputFloat3("Scale", glm::value_ptr(m_entity->m_transform.scale), "%.5f");

		ImGui::End();
	}
}
