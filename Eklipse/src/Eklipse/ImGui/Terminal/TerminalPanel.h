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

		Unique<Terminal>& GetTerminal() { return m_terminal; }

	private:
		Unique<Terminal> m_terminal;
	};
}