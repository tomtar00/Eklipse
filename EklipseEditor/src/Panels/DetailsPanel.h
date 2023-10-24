#pragma once
#include <Eklipse.h>
#include <Eklipse/Scene/Entity.h>

namespace Editor
{
	class DetailsPanel : public Eklipse::GuiPanel
	{
	public:
		void Setup(std::string& name);
		
	private:
		virtual void OnGUI(float deltaTime) override;

	private:
		std::string m_entityNameBuffer;
	};
}