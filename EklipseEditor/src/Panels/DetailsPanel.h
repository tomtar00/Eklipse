#pragma once
#include <Eklipse.h>
#include <Eklipse/Scene/Entity.h>

namespace Eklipse
{
	class DetailsPanel : public GuiPanel
	{
	public:
		void Setup(String& name);
		
	private:
		virtual bool OnGUI(float deltaTime) override;
		void OnEntityGUI(Entity entity);
		void OnMaterialGUI(Material* material);

	private:
		String m_entityNameBuffer;
	};
}