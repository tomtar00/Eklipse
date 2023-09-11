#include "precompiled.h"
#include "Settings.h"

#ifdef EK_PLATFORM_WINDOWS
	#include <GLFW/glfw3.h>
#endif

namespace Eklipse
{
	void RendererSettings::SetVsync(bool enabled)
	{
		s_vsyncEnabled = enabled;

#ifdef EK_PLATFORM_WINDOWS
		// TODO: what is the initial value of swapInterval???
		if (enabled)
			glfwSwapInterval(1);
		else 
			glfwSwapInterval(0);
#endif
	}
	bool RendererSettings::IsVsync()
	{
		return s_vsyncEnabled;
	}
	void RendererSettings::SetMsaaSamples(int numSamples)
	{
		// TODO: recreate tetures and framebuffers
		s_msaaSamples = numSamples;
	}
}
