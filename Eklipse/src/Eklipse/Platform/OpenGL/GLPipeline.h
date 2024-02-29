#pragma once
#include <Eklipse/Renderer/Pipeline.h>

namespace Eklipse
{
    namespace OpenGL
    {
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