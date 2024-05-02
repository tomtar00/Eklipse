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

        virtual void OnAPIHasInitialized(GraphicsAPI::Type api) override;
        virtual void OnShutdownAPI(bool quit) override;

    private:
        
        void CreateCamera(Ref<Scene> scene, float fov, glm::vec3& position, glm::vec3& rotation);
        void CreateCube(Ref<Scene> scene, glm::vec3& position, glm::vec3& scale, glm::vec3& color, float smoothness = 0, float specularProb = 0);
        void CreateTeapot(Ref<Scene> scene, glm::vec3& position, glm::vec3& scale, glm::vec3& color, float smoothness = 0, float specularProb = 0);
        void CreateCow(Ref<Scene> scene, glm::vec3& position, glm::vec3& scale, glm::vec3& color, float smoothness = 0, float specularProb = 0);

        Ref<Scene> SetupScene_CubeTensor(uint32_t dimension);
        Ref<Scene> SetupScene_2();
        Ref<Scene> SetupScene_3();

        void SwitchScene(Ref<Scene> scene);
        void SwitchScene(int sceneIndex);

        void ControlCamera(float deltaTime);

    private:
        Ref<Shader> m_shader3D;

        Ref<Mesh> m_cubeMesh;
        Ref<Material> m_cubeMaterial;
        glm::vec3 m_cubeColor;

        Ref<Mesh> m_teapotMesh;
        Ref<Material> m_teapotMaterial;
        glm::vec3 m_teapotColor;

        Ref<Mesh> m_cowMesh;
        Ref<Material> m_cowMaterial;
        glm::vec3 m_cowColor;

        Entity m_cameraEntity;

        float m_cameraSpeed = 5.0f;
        float m_cameraSensitivity = 0.06f;
        bool m_cursorDisabled = false;
        int m_currentScene = 0;

        bool m_drawBVH = false;
    };
}