#include "precompiled.h"
#include "GLComputeShader.h"

#include <glad/glad.h>

namespace Eklipse
{
    namespace OpenGL
    {
        GLComputeShader::GLComputeShader(const Path& filePath, const AssetHandle handle) : ComputeShader(filePath, handle)
        {
        }
        void GLComputeShader::Dispatch(uint32_t x, uint32_t y, uint32_t z) const
        {
            glDispatchCompute(x, y, z);
        }
    }
}
