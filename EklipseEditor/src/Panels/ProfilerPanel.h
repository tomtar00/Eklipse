#pragma once
#include <Eklipse.h>

namespace Eklipse
{
	class ProfilerPanel : public GuiPanel
	{
	public:
		virtual bool OnGUI(float deltaTime) override;
	private:
		bool m_ascendingSort = false;
		uint32_t m_columnIndex = -1;
	};
}