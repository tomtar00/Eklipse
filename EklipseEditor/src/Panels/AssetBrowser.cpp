#include "AssetBrowser.h"
#include <Eklipse/Project/Project.h>
#include <Eklipse/Scene/Assets.h>

namespace Editor
{
	void AssetBrowser::Init()
	{
		m_folderIcon = Eklipse::GuiIcon::Create("Assets/Icons/folder.png");
		m_fileIcon = Eklipse::GuiIcon::Create("Assets/Icons/file.png");
	}

	void AssetBrowser::OnGUI(float deltaTime)
	{
		ImGui::Begin("Browser");
		if (m_workingDirPath.empty())
		{
			ImGui::Text("No project loaded");
			ImGui::End();
			return;
		}

		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::MenuItem("Create Material"))
			{
				std::string materialName = "/Material.ekmt";
				Eklipse::Assets::GetMaterial(m_currentPath.string() + materialName, Eklipse::Operation::READ_WRITE);
			}

			ImGui::EndPopup();
		}

		if (m_currentPath != std::filesystem::path(m_workingDirPath))
		{
			if (ImGui::Button("<"))
			{
				m_currentPath = m_currentPath.parent_path();
			}
		}

		static float padding = 32.0f;
		static float thumbnailSize = 64.0f;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1) columnCount = 1;

		ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16, 128);
		ImGui::SliderFloat("Padding", &padding, 0, 32);

		ImGui::Columns(columnCount, 0, false);

		for (auto& directoryEntry : std::filesystem::directory_iterator(m_currentPath))
		{
			const auto& path = directoryEntry.path();
			std::string filenameString = path.filename().string();

			ImGui::PushID(filenameString.c_str());
			Eklipse::Ref<Eklipse::GuiIcon> icon = directoryEntry.is_directory() ? m_folderIcon : m_fileIcon;
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			ImGui::ImageButton((ImTextureID)icon->GetID(), { thumbnailSize, thumbnailSize });

			if (ImGui::BeginDragDropSource())
			{
				std::filesystem::path relativePath(path);
				const wchar_t* itemPath = relativePath.c_str();
				ImGui::SetDragDropPayload("ASSET_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
				ImGui::EndDragDropSource();
			}

			ImGui::PopStyleColor();
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (directoryEntry.is_directory())
					m_currentPath /= path.filename();
			}
			ImGui::TextWrapped(filenameString.c_str());
			ImGui::NextColumn();
			ImGui::PopID();
		}

		ImGui::Columns(1);

		ImGui::End();
	}
	void AssetBrowser::OnContextChanged()
	{
		m_workingDirPath = Eklipse::Project::GetActive()->GetProjectDirectory();
		m_currentPath = m_workingDirPath;
	}
}