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
        virtual void OnUpdate(float deltaTime) override;

        virtual void OnAPIHasInitialized(ApiType api) override;
        virtual void OnShutdownAPI(bool quit) override;

    private:
        void InitQuad();
        void InitShader();
        void InitMaterial();

        void ResetPixelBuffer();
        void ControlCamera(float deltaTime);

    private:
        String m_shaderPath;
        Ref<Shader> m_rayShader;
        Ref<Material> m_rayMaterial;
        Ref<VertexArray> m_fullscreenVA;

        Camera m_camera;
        Transform m_cameraTransform;
        bool m_controlCamera;
        float m_cameraSpeed;
        float m_cameraSensitivity;
        bool m_cursorDisabled;

        // Data
        int m_frames;
        int m_raysPerPixel;
        int m_maxBounces;

        // Background
        glm::vec3 m_skyColorHorizon;
        glm::vec3 m_skyColorZenith;
        glm::vec3 m_groundColor;
        glm::vec3 m_sunColor;
        glm::vec3 m_sunDirection;
        float m_sunFocus;
        float m_sunIntensity;
    };
}