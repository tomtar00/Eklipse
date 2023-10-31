#pragma once
#include <Eklipse.h>
#include <filesystem>
#include <Eklipse/Renderer/Texture.h>

namespace Editor
{
	class AssetBrowser : public Eklipse::GuiPanel
	{
	public:
		void Init();
		void OnGUI(float deltaTime) override;
		void OnContextChanged();

	private:
		std::filesystem::path m_currentPath;
		std::filesystem::path m_workingDirPath;

		Eklipse::Ref<Eklipse::GuiIcon> m_folderIcon;
		Eklipse::Ref<Eklipse::GuiIcon> m_fileIcon;
	};
}