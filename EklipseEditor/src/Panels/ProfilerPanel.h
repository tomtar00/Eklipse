#pragma once
#include <Eklipse.h>

namespace Eklipse
{
	class ProfilerPanel : public GuiPanel
	{
	public:
		virtual bool OnGUI(float deltaTime) override;

		void OnPlay();
		void OnStop();
		void OnPause();
		void OnResume();

	private:
		bool m_ascendingSort = false;
		int32_t m_columnIndex = -1;
	};
}