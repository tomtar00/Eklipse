#include "precompiled.h"
#include "GLPipeline.h"

namespace Eklipse
{
    namespace OpenGL
    {
        GLPipeline::GLPipeline(const Pipeline::Config& config) : Pipeline(config)
        {
            EK_CORE_PROFILE();
            Build();
        }
        void GLPipeline::Build()
        {
        }
        void GLPipeline::Bind()
        {
            EK_CORE_PROFILE();
            m_config.shader->Bind();
        }
        void GLPipeline::Dispose()
        {
        }
    }
}