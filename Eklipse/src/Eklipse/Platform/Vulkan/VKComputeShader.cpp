#include "precompiled.h"
#include "VK.h"
#include "VKComputeShader.h"

#include <vulkan/vulkan.h>

namespace Eklipse
{
    namespace Vulkan
    {
        VKComputeShader::VKComputeShader(const String& filePath) : ComputeShader(filePath)
        {
        }
        void VKComputeShader::Dispatch(uint32_t x, uint32_t y, uint32_t z) const
        {
            vkCmdDispatch(g_currentCommandBuffer, x, y, z);
        }
    }
}