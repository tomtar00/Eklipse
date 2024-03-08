#pragma once
#include <Eklipse.h>

namespace Eklipse
{
    class ComputeLayer : public Layer
    {
    public:
        virtual void OnAttach() override;
        virtual void OnEvent(Event& event) override;

        virtual void OnGUI(float deltaTime) override;
        virtual void OnUpdate(float deltaTime) override;
        virtual void OnCompute(float deltaTime) override;
        virtual void OnRender(float deltaTime) override;

        virtual void OnAPIHasInitialized(GraphicsAPI::Type api) override;
        virtual void OnShutdownAPI(bool quit) override;

    private:
        void InitQuad();
        void InitShader();
        void InitMaterial();
        void InitMeshes();
        void InitComputeShader();

        void ResetPixelBuffer();
        void ControlCamera(float deltaTime);

    private:
        Ref<ComputeShader> m_computeShader;
        Ref<Shader> m_rayShader;
        Ref<Material> m_rayMaterial;
        Ref<VertexArray> m_fullscreenVA;

        uint32_t m_numTotalVertices;
        uint32_t m_numTotalIndices;
        uint32_t m_numTotalMeshes;
        Ref<Mesh> m_cubeMesh;
        Ref<Scene> m_scene;

        Camera m_camera;
        Transform m_cameraTransform;
        bool m_controlCamera;
        float m_cameraSpeed;
        float m_cameraSensitivity;
        bool m_cursorDisabled;

        int m_frameIndex = 0;
        RayTracingContext::Settings m_rtSettings;
    };
}