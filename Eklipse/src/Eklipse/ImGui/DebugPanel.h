#pragma once
#include "ImGuiLayer.h"

namespace Eklipse
{
	class EK_API DebugPanel : public GuiPanel
	{
	public:
		DebugPanel() = default;

		bool OnGUI(float deltaTime) override;
		void AppendDrawRequest(const std::function<void()>& drawRequest);

	private:
		Vec<std::function<void()>> m_drawRequests;
	};
}