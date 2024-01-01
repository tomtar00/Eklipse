#pragma once
#include <Eklipse.h>
#include <Eklipse/Scene/Entity.h>

namespace Eklipse
{
	class DetailsPanel : public GuiPanel
	{
	public:
		void Setup(std::string& name);
		
	private:
		virtual bool OnGUI(float deltaTime) override;
		void OnEntityGUI(Entity entity);
		void OnMaterialGUI(Material* material);

	private:
		std::string m_entityNameBuffer;
	};
}