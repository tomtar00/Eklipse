#pragma once
#include "ImGuiLayer.h"

namespace Eklipse
{
	class DebugPanel : public GuiPanel
	{
	public:
		DebugPanel() = default;
		virtual ~DebugPanel() = default;

		bool OnGUI(float deltaTime) override;
		void AppendDrawRequest(const std::function<void()>& drawRequest) { m_drawRequests.push_back(drawRequest); }
		void DrawAssetLibrary(const char* name, Eklipse::Ref<Eklipse::AssetLibrary> assetLibrary);

	private:
		std::vector<std::function<void()>> m_drawRequests;
	};
}