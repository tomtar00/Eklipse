#pragma once
#include <Eklipse.h>

namespace Eklipse
{
    class RTLayer : public Layer
    {
    public:
        virtual void OnAttach() override;
        virtual void OnRender() override;

        virtual void OnGUI(float deltaTime) override;

        virtual void OnAPIHasInitialized(ApiType api) override;
        virtual void OnShutdownAPI(bool quit) override;

    private:
        Ref<Shader> m_rayShader;
        Ref<Material> m_rayMaterial;
        Ref<VertexArray> m_fullscreenVA;
    };
}