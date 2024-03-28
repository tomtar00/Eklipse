#include "precompiled.h"
#include "GLPipeline.h"

namespace Eklipse
{
    namespace OpenGL
    {
        GLint g_currentTopologyMode = GL_TRIANGLES;

        static GLint PipelineModeToGL(Pipeline::TopologyMode mode)
        {
            switch (mode)
            {
                case Pipeline::TopologyMode::Triangle:  return GL_TRIANGLES;
                case Pipeline::TopologyMode::Line:      return GL_LINES;
            }
            EK_ASSERT(false, "Unknown pipeline mode!");
            return 0;
        }

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
            g_currentTopologyMode = PipelineModeToGL(m_config.topologyMode);
        }
        void GLPipeline::Dispose()
        {
        }
    }
}