#pragma once
#include <Eklipse.h>

namespace Eklipse
{
    class SandboxLayer : public Layer
    {
    public:
        virtual void OnAttach() override;
        virtual void OnUpdate(float deltaTime) override;

        void OnAPIHasInitialized(ApiType api);
        void OnShutdownAPI();
    };
}