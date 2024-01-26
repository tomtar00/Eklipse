#pragma once
#include "ImGuiLayer.h"

namespace Eklipse
{
	class EK_API DebugPanel : public GuiPanel
	{
	public:
		DebugPanel() = default;
		virtual ~DebugPanel() = default;

		bool OnGUI(float deltaTime) override;
		void AppendDrawRequest(const std::function<void()>& drawRequest);
		void DrawAssetLibrary(const char* name, Ref<AssetLibrary> assetLibrary);

	private:
		std::vector<std::function<void()>> m_drawRequests;

		// Using it this way, because string() method of filesystem::path returns string copy
		std::vector<std::pair<const char*, String>> m_projectConfigData;
	};
}