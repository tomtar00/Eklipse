#pragma once
#include "../ImGuiLayer.h"
#include <Eklipse/Terminal/Terminal.h>

namespace Eklipse
{
	class TerminalPanel : public GuiPanel
	{
	public:
		TerminalPanel();
		bool OnGUI(float deltaTime) override;

		Terminal& GetTerminal();

	private:
		Terminal m_terminal;
	};
}