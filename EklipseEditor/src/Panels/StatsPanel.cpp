#include "StatsPanel.h"

#include <Eklipse/Utils/Stats.h>
#include "EditorLayer.h"

namespace Editor
{
	void StatsPanel::OnGUI(float deltaTime)
	{
		EK_PROFILE();

		ImGui::Begin("Stats");

		auto& stats = Eklipse::Stats::Get();

		ImGui::Text("FPS: %f", stats.fps);
		ImGui::Text("Frame time: %f ms", stats.frameTime * 1000);
		ImGui::PlotLines("Frame time", stats.frameTimes.data(), stats.frameTimes.size());
		ImGui::Text("Draw calls: %d", stats.drawCalls);
		ImGui::Text("Total vertices: %d", stats.numVertices);

		if (Eklipse::Project::GetActive() != nullptr)
			DrawAssetLibrary("Project assets", Eklipse::Project::GetActive()->GetAssetLibrary());
		DrawAssetLibrary("Editor assets", EditorLayer::Get().GetAssetLibrary());

		ImGui::End();
	}
	void StatsPanel::DrawAssetLibrary(const char* name, Eklipse::Ref<Eklipse::AssetLibrary> assetLibrary)
	{
		if (ImGui::CollapsingHeader(name))
		{
			ImGui::Text("Meshes");
			ImGui::Indent();
			for (auto&& [name, mesh] : assetLibrary->GetMeshCache())
			{
				ImGui::Text("%s (%p)", name.c_str(), mesh.get());
			}
			ImGui::Unindent();
			ImGui::Text("Textures");
			ImGui::Indent();
			for (auto&& [name, texture] : assetLibrary->GetTextureCache())
			{
				ImGui::Text("%s (%p)", name.c_str(), texture.get());
			}
			ImGui::Unindent();
			ImGui::Text("Shaders");
			ImGui::Indent();
			for (auto&& [name, shader] : assetLibrary->GetShaderCache())
			{
				ImGui::Text("%s (%p)", name.c_str(), shader.get());
			}
			ImGui::Unindent();
			ImGui::Text("Materials");
			ImGui::Indent();
			for (auto&& [name, material] : assetLibrary->GetMaterialCache())
			{
				ImGui::Text("%s (%p)", name.c_str(), material.get());
			}
			ImGui::Unindent();
		}
	}
}