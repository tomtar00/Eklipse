#include "StatsPanel.h"

#include <Eklipse/Utils/Stats.h>
#include "EditorLayer.h"

namespace Eklipse
{
	bool StatsPanel::OnGUI(float deltaTime)
	{
		EK_PROFILE();

		if (!GuiPanel::OnGUI(deltaTime)) return false;

		ImGui::Begin("Stats");

		auto& stats = Stats::Get();

		ImGui::PlotLines("##Frametime", stats.frameTimes.data(), stats.frameTimes.size());
		ImGui::Text("Frame time: %f ms", stats.frameTime * 1000);
		ImGui::Text("FPS: %f", stats.fps);

		ImGui::Separator();

		ImGui::Text("Draw calls: %d", stats.drawCalls);
		ImGui::Text("Total vertices: %d", stats.numVertices);

		ImGui::End();

		return true;
	}
}