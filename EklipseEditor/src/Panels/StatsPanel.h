#pragma once
#include <Eklipse.h>

namespace Editor
{
	class StatsPanel : public Eklipse::GuiPanel
	{
	public:
		virtual void OnGUI(float deltaTime) override;
		
		void DrawAssetLibrary(const char* name, Eklipse::Ref<Eklipse::AssetLibrary> assetLibrary);
	};
}