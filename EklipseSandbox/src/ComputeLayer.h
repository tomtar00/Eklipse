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
        void InitMaterial();
        void ResetPixelBuffer();
        void AddMesh(const AssetHandle meshHandle, const String& name);
        void ReconstructSceneBuffers();
        
        void ControlCamera(float deltaTime);

    private:
        Ref<Material> m_material;
        Ref<VertexArray> m_fullscreenVA;
        Ref<Scene> m_scene;

        Ref<ComputeShader> m_transComputeShader;
        Ref<ComputeShader> m_boundsComputeShader;

        uint32_t m_numTotalVertices;
        uint32_t m_numTotalIndices;
        uint32_t m_numTotalSpheres;
        uint32_t m_numTotalMeshes;

        AssetHandle m_transComputeShaderHandle;
        AssetHandle m_boundsComputeShaderHandle;
        AssetHandle m_shaderHandle;
        AssetHandle m_cubeMeshHandle;
        AssetHandle m_cylinderMeshHandle;
        AssetHandle m_teapotMeshHandle;
        AssetHandle m_suzanneMeshHandle;

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