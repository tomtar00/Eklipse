#pragma once
#include <Eklipse.h>

namespace Eklipse
{
    class SandboxLayer : public Layer
    {
    public:
        virtual void OnAttach() override;
        virtual void OnUpdate(float deltaTime) override;

        virtual void OnAPIHasInitialized(ApiType api) override;
        virtual void OnShutdownAPI(bool quit) override;
    };
}