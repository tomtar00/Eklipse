#pragma once
#include <Eklipse.h>

namespace Editor
{
	class EntitiesPanel : public Eklipse::GuiPanel
	{
	public:
		virtual bool OnGUI(float deltaTime) override;
		void SetContext(Eklipse::Ref<Eklipse::Scene> scene);

	private:
		Eklipse::Ref<Eklipse::Scene> m_sceneContext = nullptr;
	};
}