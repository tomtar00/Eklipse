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
			DrawAssetLibrary("App assets", Application::Get().GetAssetLibrary());
			if (Project::GetActive() != nullptr)
			{
				DrawAssetLibrary("Project assets", Project::GetActive()->GetAssetLibrary());
			}
		}

		if (Project::GetActive() != nullptr && ImGui::CollapsingHeader("Project"))
		{
			auto project = Project::GetActive();

			ImGui::Indent();

			ImGui::SeparatorText("Config");
			if (ImGui::BeginTable("Config##Table", 2))
			{
				auto& config = project->GetConfig();
				
				m_projectConfigData =
				{
					{ "Name",					config.name },
					{ "Assets path",			config.assetsDirectoryPath.string() },
					{ "Start scene path",		config.startScenePath.string() },
					{ "Scripts path",			config.scriptsDirectoryPath.string() },
					{ "Scripts source path",	config.scriptsSourceDirectoryPath.string() },
					{ "Scripts build path",		config.scriptBuildDirectoryPath.string() },
				};

				for (int i = 0; i < m_projectConfigData.size(); ++i)
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::TextUnformatted(m_projectConfigData[i].first);
					ImGui::TableSetColumnIndex(1);
					ImGui::TextUnformatted(m_projectConfigData[i].second.c_str());
				}

				ImGui::EndTable();
			}

			ImGui::SeparatorText("Scripts");
			for (auto&& [name, config] : project->GetScriptManager()->GetClasses())
			{
				if (ImGui::CollapsingHeader(name.c_str()))
				{
					ImGui::Indent();
					ImGui::Text("Create function exists: %s", (config.create != nullptr ? "true" : "false"));
					for (auto&& [name, member] : config.members)
					{
						ImGui::Text("Name: %s Type: %s Offset: %d", name.c_str(), member.type.c_str(), member.offset);
					}
					ImGui::Unindent();
				}
			}

			ImGui::SeparatorText("Dynamic library");
			if (ImGui::BeginTable("Library##Table", 2))
			{
				std::vector<std::pair<const char*, const char*>> data =
				{
					{ "Library loaded",			(project->GetScriptManager()->IsLibraryLoaded() ? "true" : "false")	},
					{ "Library state",			project->GetScriptManager()->GetState().c_str()						},
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

			if (ImGui::Button("Recompile all"))
			{
				project->GetScriptManager()->RecompileAll();
			}

			ImGui::Unindent();
		}

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