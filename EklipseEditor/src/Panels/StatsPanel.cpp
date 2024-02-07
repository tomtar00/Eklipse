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

		ImGui::DrawProperty("frame_time", "Frame time", [&]() {
			ImGui::PlotLines("##Frametime", stats.frameTimes.data(), stats.frameTimes.size());
			ImGui::Text("%f ms", stats.frameTime * 1000);
		});
		ImGui::DrawProperty("fps", "FPS", [&]() {
			ImGui::Text("%f", stats.fps);
		});
		ImGui::DrawProperty("draw_calls", "Draw calls", [&]() {
			ImGui::Text("%d", stats.drawCalls);
		});
		ImGui::DrawProperty("total_vert", "Total vertices", [&]() {
			ImGui::Text("%d", stats.numVertices);
		});

		ImGui::End();

		return true;
	}
}