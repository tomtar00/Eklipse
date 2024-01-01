#pragma once
#include <Eklipse.h>

namespace Eklipse
{
	class EntitiesPanel : public GuiPanel
	{
	public:
		virtual bool OnGUI(float deltaTime) override;
		void SetContext(Ref<Scene> scene);

	private:
		Ref<Scene> m_sceneContext = nullptr;
	};
}