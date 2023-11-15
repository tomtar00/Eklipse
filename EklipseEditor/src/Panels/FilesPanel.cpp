#include "FilesPanel.h"
#include "EditorLayer.h"
#include <Eklipse/Project/Project.h>
#include <misc/cpp/imgui_stdlib.h>

namespace Editor
{
	void FilesPanel::LoadResources()
	{
		m_folderIcon = Eklipse::GuiIcon::Create(EditorLayer::Get().GetAssetLibrary(), "Assets/Icons/folder.png");
		m_fileIcon = Eklipse::GuiIcon::Create(EditorLayer::Get().GetAssetLibrary(), "Assets/Icons/file.png");
	}

	void FilesPanel::OnGUI(float deltaTime)
	{
		ImGui::Begin("Files");
		if (m_workingDirPath.empty())
		{
			ImGui::Text("No project loaded");
			ImGui::SameLine();
			if (ImGui::Button("Open Project"))
			{
				EditorLayer::Get().OpenProject();
			}
			ImGui::End();
			return;
		}

		// Right click menu
		static char* popupName = nullptr;
		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::MenuItem("Create Folder"))
			{
				popupName = "Create New Folder";
			}
			if (ImGui::MenuItem("Create Shader"))
			{
				popupName = "Create New Shader";
			}
			if (ImGui::MenuItem("Create Material"))
			{
				popupName = "Create New Material";
			}

			ImGui::EndPopup();
		}

		// Popups creation
		{
			if (popupName != nullptr)
			{
				ImGui::OpenPopup(popupName);
				popupName = nullptr;
			}

			// create folder
			if (ImGui::BeginPopupModal("Create New Folder", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
			{
				static std::string folderName = "NewFolder";

				ImGui::InputText("Folder Name", &folderName);

				if (ImGui::Button("Create") && !folderName.empty())
				{
					std::filesystem::create_directory(m_currentPath / folderName);
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel"))
				{
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}

			if (ImGui::BeginPopupModal("Create New Shader", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
			{
				static std::string shaderName = "NewShader";
				static int templateOption = true;

				ImGui::InputText("Shader Name", &shaderName);
				ImGui::RadioButton("3D", &templateOption, 0);
				ImGui::SameLine();
				ImGui::RadioButton("2D", &templateOption, 1);

				if (ImGui::Button("Create") && !shaderName.empty())
				{
					Eklipse::Path templatePath = "//Shaders/Default3D.eksh";
					if (templateOption == 1)
						templatePath = "//Shaders/Default2D.eksh";

					CreateShader(m_currentPath / (shaderName + EK_SHADER_EXTENSION), templatePath);
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel"))
				{
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}

			if (ImGui::BeginPopupModal("Create New Material", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
			{
				static std::string materialName = "NewMaterial";
				static std::string shaderPath = "//Shaders/Default3D.eksh";

				ImGui::InputText("Material Name", &materialName);
				if (ImGui::BeginCombo("Shader", shaderPath.c_str()))
				{
					for (auto& shader : Eklipse::Project::GetActive()->GetAssetLibrary()->GetShaderCache())
					{
						if (ImGui::Selectable(shader.first.c_str()))
						{
							shaderPath = shader.first;
						}
					}
					ImGui::EndCombo();
				}

				if (ImGui::Button("Create") && !materialName.empty())
				{
					CreateMaterial(m_currentPath / (materialName + EK_MATERIAL_EXTENSION), shaderPath);
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel"))
				{
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
		}

		// Breadcrumbs
		{
			std::vector<std::filesystem::path> paths;
			std::filesystem::path path = m_currentPath;
			while (path != std::filesystem::path(m_workingDirPath).parent_path())
			{
				paths.push_back(path);
				path = path.parent_path();
			}

			for (int i = (int)paths.size() - 1; i >= 0; i--)
			{
				ImGui::Text("/");
				ImGui::SameLine();
				if (ImGui::Button(paths[i].filename().string().c_str()))
				{
					m_currentPath = paths[i];
				}
				ImGui::SameLine();
			}
		}

		ImGui::NewLine();
		ImGui::Separator();

		static float padding = 32.0f;
		static float thumbnailSize = 64.0f;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1) columnCount = 1;

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
				if (directoryEntry.path().extension() == EK_MATERIAL_EXTENSION)
				{
					DetailsSelectionInfo info{};
					info.type = SelectionType::Material;
					info.material = Eklipse::Project::GetActive()->GetAssetLibrary()->GetMaterial(directoryEntry.path()).get();
					EditorLayer::Get().SetSelection(info);
				}
			}

			ImGui::Text(filenameString.c_str());
			ImGui::NextColumn();
			ImGui::PopID();
		}

		ImGui::Columns(1);
		//ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16, 128);

		ImGui::End();
	}
	void FilesPanel::OnContextChanged()
	{
		m_workingDirPath = Eklipse::Project::GetActive()->GetConfig().assetsDirectoryPath;
		m_currentPath = m_workingDirPath;
	}
	void FilesPanel::CreateMaterial(const Eklipse::Path& dstPath, const Eklipse::Path& shaderPath)
	{
		Eklipse::Project::GetActive()->GetAssetLibrary()->GetMaterial(dstPath, shaderPath);
	}
	void FilesPanel::CreateShader(const Eklipse::Path& dstPath, const Eklipse::Path& templatePath)
	{
		Eklipse::CopyFileContent(dstPath, templatePath);
		Eklipse::Project::GetActive()->GetAssetLibrary()->GetShader(dstPath);
	}
}