#pragma once
#include <Eklipse/Renderer/ComputeShader.h>

namespace Eklipse
{
    namespace Vulkan
    {
        class EK_API VKComputeShader : public ComputeShader
        {
        public:
            VKComputeShader(const Path& filePath, const AssetHandle handle = 0);
            virtual void Dispatch(uint32_t x, uint32_t y, uint32_t z) const override;            
        };
    }
}