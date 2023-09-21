#pragma once
#include <Eklipse.h>

namespace Editor
{
	class ProfilerPanel : public Eklipse::ImGuiPanel
	{
	public:
		void OnGUI();
	private:
		bool m_ascendingSort = false;
		uint32_t m_columnIndex = -1;
	};
}