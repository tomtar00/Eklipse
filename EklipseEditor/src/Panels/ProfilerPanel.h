#pragma once
#include <Eklipse.h>

namespace Editor
{
	class ProfilerPanel : public Eklipse::ImGuiPanel
	{
	public:
		void OnGUI();
	};
}