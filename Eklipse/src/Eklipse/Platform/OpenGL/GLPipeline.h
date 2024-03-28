#pragma once
#include <Eklipse/Renderer/Pipeline.h>
#include <glad/glad.h>

namespace Eklipse
{
    namespace OpenGL
    {
        extern GLint g_currentTopologyMode;

        class GLPipeline : public Pipeline
        {
        public:
            GLPipeline(const Pipeline::Config& config);

            virtual void Build() override;
            virtual void Bind() override;
            virtual void Dispose() override;
        };
    }
}