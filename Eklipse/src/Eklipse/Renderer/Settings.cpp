#include "precompiled.h"
#include "Settings.h"

#include <Eklipse/Renderer/Renderer.h>

#ifdef EK_PLATFORM_WINDOWS
	#include <GLFW/glfw3.h>
#endif

namespace Eklipse
{
	void RendererSettings::SetVsync(bool enabled)
	{
		if (s_vsyncEnabled == enabled) return;

		s_vsyncEnabled = enabled;

#ifdef EK_PLATFORM_WINDOWS
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
		if (s_msaaSamples == numSamples) return;
		s_msaaSamples = numSamples;

		Renderer::OnMultiSamplingChanged(numSamples);
	}
}