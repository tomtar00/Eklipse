#include "precompiled.h"
#include "VK.h"
#include "VKComputeShader.h"

#include <vulkan/vulkan.h>

namespace Eklipse
{
    namespace Vulkan
    {
        VKComputeShader::VKComputeShader(const Path& filePath, const AssetHandle handle) : ComputeShader(filePath, handle)
        {
        }
        void VKComputeShader::Dispatch(uint32_t x, uint32_t y, uint32_t z) const
        {
            vkCmdDispatch(g_currentCommandBuffer, x, y, z);
        }
    }
}