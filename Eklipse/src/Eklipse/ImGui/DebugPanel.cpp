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
			auto project = Eklipse::Project::GetActive();

			ImGui::Indent();

			ImGui::SeparatorText("Config");
			if (ImGui::BeginTable("Config##Table", 2))
			{
				auto& config = project->GetConfig();
				
				std::vector<std::pair<const char*, const char*>> data =
				{
					{ "Name",					config.name.c_str() },
					{ "Assets path",			config.assetsDirectoryPath.full_c_str() },
					{ "Start scene path",		config.startScenePath.full_c_str() },
					{ "Scripts path",			config.scriptsDirectoryPath.full_c_str() },
					{ "Scripts source path",	config.scriptsSourceDirectoryPath.full_c_str() },
					{ "Scripts build path",		config.buildDirectoryPath.full_c_str() },
				};

				for (int i = 0; i < data.size(); ++i) 
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::TextUnformatted(data[i].first);
					ImGui::TableSetColumnIndex(1);
					ImGui::TextUnformatted(data[i].second);
				}

				ImGui::EndTable();
			}

			ImGui::SeparatorText("Scripts");
			for (auto&& [name, config] : project->GetScriptClasses())
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

			ImGui::SeparatorText("Dynamic library");
			ImGui::Text("Library loaded: %s", (project->GetScriptModule().IsLibraryLoaded() ? "true" : "false"));

			if (ImGui::Button("Load library"))
			{
				project->GetScriptModule().Load(project);
			}
			ImGui::SameLine();
			if (ImGui::Button("Unload library"))
			{
				project->GetScriptModule().Unload();
			}
			ImGui::SameLine();
			if (ImGui::Button("Recompile all"))
			{
				project->GetScriptModule().RecompileAll();
			}

			ImGui::Unindent();
		}

		for (auto& drawRequest : m_drawRequests)
			drawRequest();

		ImGui::End();
        return true;
    }
}