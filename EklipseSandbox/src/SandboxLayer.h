#pragma once
#include <Eklipse.h>

namespace Eklipse
{
    class SandboxLayer : public Layer
    {
    public:
        virtual void OnAttach() override;
        virtual void OnUpdate(float deltaTime) override;
        virtual void OnRender(float deltaTime) override;    
        virtual void OnGUI(float deltaTime) override;

        virtual void OnAPIHasInitialized(ApiType api) override;
        virtual void OnShutdownAPI(bool quit) override;

    private:
        void ControlCamera(float deltaTime);

    private:
        Ref<Shader> m_shader3D;
        Ref<Material> m_planeMaterial;
        glm::vec3 m_planeColor;

        Ref<Mesh> m_cubeMesh;
        Ref<Material> m_cubeMaterial;
        glm::vec3 m_cubeColor;

        Ref<Mesh> m_sphereMesh;
        Ref<Material> m_sphereMaterial;
        glm::vec3 m_sphereColor;

        Ref<Mesh> m_teapotMesh;
        Ref<Material> m_teapotMaterial;
        glm::vec3 m_teapotColor;

        Entity m_camera;
        Entity m_plane;
        Entity m_cube;
        Entity m_sphere;
        Entity m_teapot;

        float m_cameraSpeed = 5.0f;
        float m_cameraSensitivity = 0.06f;
        bool m_cursorDisabled = false;
    };
}