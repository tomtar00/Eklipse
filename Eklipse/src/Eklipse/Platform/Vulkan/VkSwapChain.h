#pragma once
#include <vulkan/vulkan.h>

namespace Eklipse
{
	namespace Vulkan
	{
		void SetupSwapchain();
		void SetupFramebuffers();
		void DisposeSwapchain();
	}
}