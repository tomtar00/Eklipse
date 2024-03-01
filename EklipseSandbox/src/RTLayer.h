#pragma once
#include <Eklipse.h>

namespace Eklipse
{
    struct RTMaterial
    {
        glm::vec3 albedo;
        float pad0;
        float smoothness;
        float specularProb;
        glm::vec3 specularColor;
        float pad1;
        glm::vec3 emissionColor;
        float pad2;
        float emissionStrength;
    };
    struct MeshInfo
    {
        uint32_t firstTriangle;
        uint32_t numTriangles;
        glm::vec3 boundMin;
        float pad0;
        glm::vec3 boundMax;
        float pad1;
        float pad[2];
        RTMaterial material;
    };
    struct Meshes
    {
        uint32_t numMeshes;
        MeshInfo* meshes;
    };

    class RTLayer : public Layer
    {
    public:
        virtual void OnAttach() override;

        virtual void OnEvent(Event& event) override;
        virtual void OnGUI(float deltaTime) override;
        virtual void OnRender(float deltaTime) override;
        virtual void OnUpdate(float deltaTime) override;

        virtual void OnAPIHasInitialized(GraphicsAPI::Type api) override;
        virtual void OnShutdownAPI(bool quit) override;

    private:
        void InitQuad();
        void InitShader();
        void InitMaterial();
        void InitMeshes();

        void ResetPixelBuffer();
        void ControlCamera(float deltaTime);

    private:
        String m_shaderPath;
        Ref<Shader> m_rayShader;
        Ref<Material> m_rayMaterial;
        Ref<VertexArray> m_fullscreenVA;

        Ref<Mesh> m_cubeMesh;
        Vec<Triangle> m_triangles;

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