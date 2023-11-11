#include "AssetBrowser.h"
#include "EditorLayer.h"
#include <Eklipse/Project/Project.h>

namespace Editor
{
	void AssetBrowser::LoadResources()
	{
		m_folderIcon = Eklipse::GuiIcon::Create(EditorLayer::Get().GetAssetLibrary(), "Assets/Icons/folder.png");
		m_fileIcon = Eklipse::GuiIcon::Create(EditorLayer::Get().GetAssetLibrary(), "Assets/Icons/file.png");
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
			if (ImGui::MenuItem("Create Shader"))
			{
				CreateShader(m_currentPath / "NewShader.eksh", "Assets/Shaders/Default3D.eksh");
			}
			if (ImGui::MenuItem("Create Material"))
			{
				CreateMaterial(m_currentPath / "NewMaterial.ekmt", "Assets/Shaders/Default3D.eksh");
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

			if (ImGui::IsItemClicked())
			{
				if (directoryEntry.path().extension() == ".ekmt")
				{
					DetailsSelectionInfo info{};
					info.type = SelectionType::Material;
					info.material = Eklipse::Project::GetActive()->GetAssetLibrary()->GetMaterial(directoryEntry.path()).get();
					EditorLayer::Get().SetSelection(info);
				}
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
	void AssetBrowser::CreateMaterial(const Eklipse::Path& dstPath, const Eklipse::Path& shaderTemplatePath)
	{
		Eklipse::Path shaderPath = "//Shaders/" + (shaderTemplatePath.path().stem().string() + ".eksh");
		if (!std::filesystem::exists(shaderPath))
			CreateShader(shaderPath, shaderTemplatePath);

		Eklipse::Project::GetActive()->GetAssetLibrary()->GetMaterial(dstPath, shaderPath);
	}
	void AssetBrowser::CreateShader(const Eklipse::Path& dstPath, const Eklipse::Path& templatePath)
	{
		Eklipse::CopyFileContent(dstPath, templatePath);
		Eklipse::Project::GetActive()->GetAssetLibrary()->GetShader(dstPath);
	}
}