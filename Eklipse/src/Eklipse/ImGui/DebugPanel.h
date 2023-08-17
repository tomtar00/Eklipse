#ifdef EK_INCLUDE_DEBUG_LAYER
#pragma once
#include "ImGuiLayer.h"

namespace Eklipse
{
	class DebugPanel : public ImGuiPanel
	{
	public:
		virtual void OnGUI() override;
	};
}
#endif