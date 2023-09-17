#include "StatsPanel.h"

#include <Eklipse/Utils/Stats.h>

namespace Editor
{
	void StatsPanel::OnGUI()
	{
		ImGui::Begin("Stats");

		auto& stats = Eklipse::Stats::Get();

		ImGui::Text("FPS: %f", stats.fps);
		ImGui::Text("Frame time: %f ms", stats.frameTime * 1000);
		ImGui::PlotLines("Frame time", stats.frameTimes.data(), stats.frameTimes.size());
		ImGui::Text("Draw calls: %d", stats.drawCalls);
		ImGui::Text("Total vertices: %d", stats.numVertices);

		ImGui::End();
	}
}