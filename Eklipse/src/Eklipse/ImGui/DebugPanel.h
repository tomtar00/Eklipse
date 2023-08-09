#pragma once
#include "ImGuiLayer.h"

namespace Eklipse
{
	class DebugPanel : public ImGuiPanel
	{
	public:
		virtual void OnGUI() override;
	};

	class DebugPanel2 : public ImGuiPanel
	{
	public:
		virtual void OnGUI() override;
	};
}