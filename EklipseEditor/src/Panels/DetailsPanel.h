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
		virtual bool OnGUI(float deltaTime) override;
		void OnEntityGUI(Eklipse::Entity entity);
		void OnMaterialGUI(Eklipse::Material* material);

	private:
		std::string m_entityNameBuffer;
	};
}