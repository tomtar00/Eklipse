#pragma once
#include <Eklipse.h>

namespace Eklipse
{
    class RTLayer : public Layer
    {
    public:
        virtual void OnAttach() override;

        virtual void OnEvent(Event& event) override;
        virtual void OnGUI(float deltaTime) override;
        virtual void OnRender(float deltaTime) override;

        virtual void OnAPIHasInitialized(ApiType api) override;
        virtual void OnShutdownAPI(bool quit) override;

    private:
        void InitQuad();
        void InitShader();
        void InitMaterial();

        void ResetPixelBuffer();

    private:
        String m_shaderPath;
        Ref<Shader> m_rayShader;
        Ref<Material> m_rayMaterial;
        Ref<VertexArray> m_fullscreenVA;
        Ref<StorageBuffer> m_pixelBuffer;

        Camera m_camera;
        Transform m_cameraTransform;
    };
}