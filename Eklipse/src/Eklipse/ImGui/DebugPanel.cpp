#include "precompiled.h"
#include "DebugPanel.h"
#include <Eklipse/Core/Application.h>

namespace Eklipse
{
    bool DebugPanel::OnGUI(float deltaTime)
    {
        if (!GuiPanel::OnGUI(deltaTime)) return false;
        
		ImGui::Begin("Debug");

		for (auto& drawRequest : m_drawRequests)
			drawRequest();

		ImGui::End();
        return true;
    }
	void DebugPanel::AppendDrawRequest(const std::function<void()>& drawRequest)
	{
		m_drawRequests.push_back(drawRequest);
	}
}