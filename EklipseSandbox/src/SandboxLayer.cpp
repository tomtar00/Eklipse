#include "SandboxLayer.h"

namespace Eklipse
{
    void SandboxLayer::OnAttach()
    {
        // Scene
        auto scene = CreateRef<Scene>();
        SceneManager::SetActiveScene(scene);

        // Camera
        m_camera = scene->CreateEntity("Camera");
        auto& cameraComp = m_camera.AddComponent<CameraComponent>();
        cameraComp.camera.m_isMain = true;
        auto& camera_transComp = m_camera.GetComponent<TransformComponent>();
        camera_transComp.transform.position = glm::vec3(0.0f, 0.0f, 5.0f);

        // Plane
        m_plane = scene->CreateEntity("Plane");
        auto& plane_transComp = m_plane.GetComponent<TransformComponent>();
        plane_transComp.transform.position = glm::vec3(0.0f, -0.5f, 0.0f);
        plane_transComp.transform.scale = glm::vec3(10.0f, 0.1f, 10.0f);
        m_plane.AddComponent<MeshComponent>();
        m_planeColor = glm::vec3(0.0f, 1.0f, 0.0f);

        // Cube
        m_cube = scene->CreateEntity("Cube");
        auto& cube_transComp = m_cube.GetComponent<TransformComponent>();
        cube_transComp.transform.position = glm::vec3(1.0f, 0.0f, 0.0f);
        m_cube.AddComponent<MeshComponent>();
        m_cubeColor = glm::vec3(0.0f, 0.0f, 1.0f);

        // Sphere
        m_sphere = scene->CreateEntity("Sphere");
        auto& sphere_transComp = m_sphere.GetComponent<TransformComponent>();
        sphere_transComp.transform.position = glm::vec3(-1.0f, 0.0f, 0.0f);
        m_sphere.AddComponent<MeshComponent>();
        m_sphereColor = glm::vec3(1.0f, 0.0f, 0.0f);

        // Teapot
        m_teapot = scene->CreateEntity("Teapot");
        auto& teapot_transComp = m_teapot.GetComponent<TransformComponent>();
        teapot_transComp.transform.position = glm::vec3(0.0f, 0.0f, 3.0f);
        m_teapot.AddComponent<MeshComponent>();
        m_teapotColor = glm::vec3(1.0f, 1.0f, 0.0f);

        scene->OnSceneStart();
    }
    void SandboxLayer::OnUpdate(float deltaTime)
    {
        if (m_cursorDisabled)
            ControlCamera(deltaTime);
    }
    void SandboxLayer::OnRender(float deltaTime)
    {
        Renderer::RenderScene(SceneManager::GetActiveScene());
    }
    void SandboxLayer::OnGUI(float deltaTime)
    {
        ImGui::Begin("Sandbox");
        ImGui::Text("FPS: %f", Stats::Get().fps);
        ImGui::Separator();
        if (ImGui::ColorEdit3("Plane Color", &m_planeColor[0]))
        {
            m_planeMaterial->SetConstant("uFragConst", "Color", &m_planeColor, sizeof(glm::vec3));
        }
        if (ImGui::ColorEdit3("Cube Color", &m_cubeColor[0]))
        {
            m_cubeMaterial->SetConstant("uFragConst", "Color", &m_cubeColor, sizeof(glm::vec3));
        }
        if (ImGui::ColorEdit3("Sphere Color", &m_sphereColor[0]))
        {
            m_sphereMaterial->SetConstant("uFragConst", "Color", &m_sphereColor, sizeof(glm::vec3));
        }
        if (ImGui::ColorEdit3("Teapot Color", &m_teapotColor[0]))
        {
            m_teapotMaterial->SetConstant("uFragConst", "Color", &m_teapotColor, sizeof(glm::vec3));
        }

        bool isHovered = ImGui::IsWindowHovered() || ImGui::IsAnyItemHovered();
        if (m_cursorDisabled && Input::IsKeyDown(Escape))
        {
            Application::Get().GetWindow()->SetCursorMode(CursorMode::Normal);
            m_cursorDisabled = false;
        }
        else if (!isHovered && !m_cursorDisabled && Input::IsMouseButtonDown(MouseCode::Button0))
        {
            Application::Get().GetWindow()->SetCursorMode(CursorMode::Disabled);
            m_cursorDisabled = true;
        }

        ImGui::End();
    }
    void SandboxLayer::OnAPIHasInitialized(ApiType api)
    {
        // Geometry
        m_shader3D = Shader::Create("Assets/Shaders/3D.glsl");

        m_planeMaterial = Material::Create(m_shader3D);
        m_planeMaterial->SetConstant("uFragConst", "Color", &m_planeColor, sizeof(glm::vec3));

        m_cubeMesh = Mesh::Create("Assets/Meshes/cube.obj");
        m_cubeMaterial = Material::Create(m_shader3D);
        m_cubeMaterial->SetConstant("uFragConst", "Color", &m_cubeColor, sizeof(glm::vec3));

        m_sphereMesh = Mesh::Create("Assets/Meshes/sphere.obj");
        m_sphereMaterial = Material::Create(m_shader3D);
        m_sphereMaterial->SetConstant("uFragConst", "Color", &m_sphereColor, sizeof(glm::vec3));

        m_teapotMesh = Mesh::Create("Assets/Meshes/teapot.obj");
        m_teapotMaterial = Material::Create(m_shader3D);
        m_teapotMaterial->SetConstant("uFragConst", "Color", &m_teapotColor, sizeof(glm::vec3));

        // Apply new assets to objects on the scene
        // Plane
        auto& planeMeshComp = SceneManager::GetActiveScene()->GetEntity(m_plane.GetUUID()).GetComponent<MeshComponent>();
        planeMeshComp.mesh = m_cubeMesh.get();
        planeMeshComp.material = m_planeMaterial.get();
        // Cube
        auto& cubeMeshComp = SceneManager::GetActiveScene()->GetEntity(m_cube.GetUUID()).GetComponent<MeshComponent>();
        cubeMeshComp.mesh = m_cubeMesh.get();
        cubeMeshComp.material = m_cubeMaterial.get();
        // Sphere
        auto& sphereMeshComp = SceneManager::GetActiveScene()->GetEntity(m_sphere.GetUUID()).GetComponent<MeshComponent>();
        sphereMeshComp.mesh = m_sphereMesh.get();
        sphereMeshComp.material = m_sphereMaterial.get();
        // Teapot
        auto& teapotMeshComp = SceneManager::GetActiveScene()->GetEntity(m_teapot.GetUUID()).GetComponent<MeshComponent>();
        teapotMeshComp.mesh = m_teapotMesh.get();
        teapotMeshComp.material = m_teapotMaterial.get();

        // Maximize the window
        Application::Get().GetWindow()->Maximize();

        // Lock the cursor
        Application::Get().GetWindow()->SetCursorMode(CursorMode::Disabled);
        m_cursorDisabled = true;
    }
    void SandboxLayer::OnShutdownAPI(bool quit)
    {
        // Dispose assets
        m_shader3D->Dispose();

        m_planeMaterial->Dispose();

        m_cubeMesh->Dispose();
        m_cubeMaterial->Dispose();

        m_sphereMesh->Dispose();
        m_sphereMaterial->Dispose();

        m_teapotMesh->Dispose();
        m_teapotMaterial->Dispose();
    }

    void SandboxLayer::ControlCamera(float deltaTime)
    {
        auto& transComp = m_camera.GetComponent<TransformComponent>();
        if (Input::IsKeyDown(W))
        {
            transComp.transform.position += transComp.GetForward() * m_cameraSpeed * deltaTime;
        }
        if (Input::IsKeyDown(S))
        {
            transComp.transform.position -= transComp.GetForward() * m_cameraSpeed * deltaTime;
        }
        if (Input::IsKeyDown(A))
        {
            transComp.transform.position -= transComp.GetRight() * m_cameraSpeed * deltaTime;
        }
        if (Input::IsKeyDown(D))
        {
            transComp.transform.position += transComp.GetRight() * m_cameraSpeed * deltaTime;
        }
        if (Input::IsKeyDown(LeftShift))
        {
            transComp.transform.position -= transComp.GetUp() * m_cameraSpeed * deltaTime;
        }
        if (Input::IsKeyDown(Space))
        {
            transComp.transform.position += transComp.GetUp() * m_cameraSpeed * deltaTime;
        }

        auto mouesDelta = Input::GetMouseDelta();
        transComp.transform.rotation.y -= mouesDelta.x * m_cameraSensitivity;
        transComp.transform.rotation.x -= mouesDelta.y * m_cameraSensitivity;
    }
}