#pragma once
#include <Eklipse.h>

namespace Eklipse
{
	class EntitiesPanel : public GuiPanel
	{
	public:
		virtual bool OnGUI(float deltaTime) override;
		void SetContext(Scene* scene);

	private:
		Scene* m_sceneContext = nullptr;
	};
}