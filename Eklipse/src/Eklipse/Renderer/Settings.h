#pragma once

namespace Eklipse
{
	class RendererSettings
	{
	public:
		static void SetVsync(bool enabled);
		static bool IsVsync();
		
		inline static int GetMsaaSamples() { return s_msaaSamples; }
		static void SetMsaaSamples(int numSamples);

	private:
		inline static bool s_vsyncEnabled = false;
		inline static int s_msaaSamples = 1;
		inline static int s_mipMapLevel = 1;
	};
}