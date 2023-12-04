#pragma once
#include <Eklipse.h>
#include <filesystem>
#include <Eklipse/Renderer/Texture.h>

namespace Editor
{
	class FilesPanel : public Eklipse::GuiPanel
	{
	public:
		void LoadResources();
		bool OnGUI(float deltaTime) override;
		void OnContextChanged();

		void CreateMaterial(const Eklipse::Path& dstPath, const Eklipse::Path& shaderPath);
		void CreateShader(const Eklipse::Path& dstPath, const Eklipse::Path& templatePath);

	private:
		std::filesystem::path m_currentPath;
		std::filesystem::path m_workingDirPath;

		Eklipse::Ref<Eklipse::GuiIcon> m_folderIcon;
		Eklipse::Ref<Eklipse::GuiIcon> m_fileIcon;
	};
}