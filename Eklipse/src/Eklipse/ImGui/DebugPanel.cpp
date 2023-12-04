#include "precompiled.h"
#include "DebugPanel.h"
#include <Eklipse/Core/Application.h>

namespace Eklipse
{
	void DebugPanel::DrawAssetLibrary(const char* name, Eklipse::Ref<Eklipse::AssetLibrary> assetLibrary)
	{
		ImGui::Indent();
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
		ImGui::Unindent();
	}

    bool DebugPanel::OnGUI(float deltaTime)
    {
        if (!GuiPanel::OnGUI(deltaTime)) return false;
        
		ImGui::Begin("Debug");

		if (ImGui::CollapsingHeader("Assets"))
		{
			DrawAssetLibrary("App assets", Eklipse::Application::Get().GetAssetLibrary());
			if (Eklipse::Project::GetActive() != nullptr)
			{
				DrawAssetLibrary("Project assets", Eklipse::Project::GetActive()->GetAssetLibrary());
			}
		}

		if (Eklipse::Project::GetActive() != nullptr && ImGui::CollapsingHeader("Project"))
		{
			ImGui::Indent();
			auto& config = Eklipse::Project::GetActive()->GetConfig();
			ImGui::Text("Name: %s", config.name.c_str());
			ImGui::Text("Assets path: %s", config.assetsDirectoryPath.string().c_str());
			ImGui::Text("Start scene path: %s", config.startScenePath.string().c_str());
			ImGui::Text("Scripts path: %s", config.scriptsDirectoryPath.c_str());
			ImGui::Text("Scripts source path: %s", config.scriptsSourceDirectoryPath.c_str());
			ImGui::Text("Scripts build path: %s", config.buildDirectoryPath.c_str());

			ImGui::Text("Scripts");
			ImGui::Indent();
			for (auto&& [name, config] : Eklipse::Project::GetActive()->GetScriptClasses())
			{
				if (ImGui::CollapsingHeader(name.c_str()))
				{
					ImGui::Text("Create function exists: %s", (config.create != nullptr ? "true" : "false"));
					for (auto&& [name, member] : config.members)
					{
						ImGui::Text("Type: %s Offset: %d", member.type.c_str(), member.offset);
					}
				}
			}
			ImGui::Unindent();
			ImGui::Unindent();
		}

		for (auto& drawRequest : m_drawRequests)
			drawRequest();

		ImGui::End();
        return true;
    }
}