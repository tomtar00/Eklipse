#pragma once
#include <Eklipse.h>
#include <filesystem>
#include <Eklipse/Renderer/Texture.h>

namespace Eklipse
{
	class FilesPanel : public GuiPanel
	{
	public:
		void LoadResources();
		bool OnGUI(float deltaTime) override;
		void OnContextChanged();

		void CreateMaterial(const Path& dstPath, const Path& shaderPath);
		void CreateShader(const Path& dstPath, const Path& templatePath);

	private:
		std::filesystem::path m_currentPath;
		std::filesystem::path m_workingDirPath;

		Ref<GuiIcon> m_folderIcon;
		Ref<GuiIcon> m_fileIcon;
	};
}