#pragma once
#include <Eklipse.h>
#include <Eklipse/Scene/Entity.h>

namespace Editor
{
	class DetailsPanel : public Eklipse::ImGuiPanel
	{
	public:
		void Setup(Eklipse::Entity entity, std::string& name);
		
	private:
		void OnGUI();

	private:
		Eklipse::Entity m_entity;
		std::string m_entityNameBuffer;
	};
}