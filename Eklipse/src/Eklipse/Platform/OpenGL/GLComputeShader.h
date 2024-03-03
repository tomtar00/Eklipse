#pragma once
#include <Eklipse/Renderer/ComputeShader.h>

namespace Eklipse
{
    namespace OpenGL
    {
        class EK_API GLComputeShader : public ComputeShader
        {
        public:
            GLComputeShader(const String& filePath);
            virtual void Dispatch(uint32_t x, uint32_t y, uint32_t z) const override;
        };
    }
}