#pragma once
#include <Eklipse.h>

namespace Editor
{
	class ProfilerPanel : public Eklipse::GuiPanel
	{
	public:
		virtual bool OnGUI(float deltaTime) override;
	private:
		bool m_ascendingSort = false;
		uint32_t m_columnIndex = -1;
	};
}