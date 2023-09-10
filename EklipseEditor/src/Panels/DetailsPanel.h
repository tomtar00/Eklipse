#pragma once
#include <Eklipse.h>

namespace Editor
{
	class DetailsPanel : public Eklipse::ImGuiPanel
	{
	public:
		void Setup(Eklipse::Entity* entity);
		
	private:
		void OnGUI();

	private:
		Eklipse::Entity* m_entity = nullptr;
		std::string m_entityNameBuffer;
	};
}