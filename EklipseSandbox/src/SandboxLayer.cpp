#include "SandboxLayer.h"

namespace Eklipse
{
    static Ref<Shader> s_shader3D;

    static Ref<Material> s_planeMaterial;

    static Ref<Mesh> s_cubeMesh;
    static Ref<Material> s_cubeMaterial;

    static Ref<Mesh> s_sphereMesh;
    static Ref<Material> s_sphereMaterial;

    static Entity s_camera;
    static Entity s_plane;
    static Entity s_cube;
    static Entity s_sphere;

    static float cameraSpeed = 5.0f;
    static float cameraSensitivity = 0.06f;
    static bool cursorDisabled = false;

    static Ref<VertexArray> s_vertexArray;
    static Ref<Shader> s_rayShader;

    static void ControlCamera(float deltaTime)
    {
        auto& transComp = s_camera.GetComponent<TransformComponent>();
        if (Input::IsKeyDown(W))
        {
            transComp.transform.position += transComp.GetForward() * cameraSpeed * deltaTime;
        }
        if (Input::IsKeyDown(S))
        {
            transComp.transform.position -= transComp.GetForward() * cameraSpeed * deltaTime;
        }
        if (Input::IsKeyDown(A))
        {
            transComp.transform.position -= transComp.GetRight() * cameraSpeed * deltaTime;
        }
        if (Input::IsKeyDown(D))
        {
            transComp.transform.position += transComp.GetRight() * cameraSpeed * deltaTime;
        }
        if (Input::IsKeyDown(LeftShift))
        {
            transComp.transform.position -= transComp.GetUp() * cameraSpeed * deltaTime;
        }
        if (Input::IsKeyDown(Space))
        {
            transComp.transform.position += transComp.GetUp() * cameraSpeed * deltaTime;
        }

        auto mouesDelta = Input::GetMouseDelta();
        transComp.transform.rotation.y -= mouesDelta.x * cameraSensitivity;
        transComp.transform.rotation.x -= mouesDelta.y * cameraSensitivity;
    }

    void SandboxLayer::OnAttach()
    {
        // Scene
        auto scene = CreateRef<Scene>();
        SceneManager::SetActiveScene(scene);

        // Camera
        s_camera = scene->CreateEntity("Camera");
        auto& cameraComp = s_camera.AddComponent<CameraComponent>();
        cameraComp.camera.m_isMain = true;
        auto& camera_transComp = s_camera.GetComponent<TransformComponent>();
        camera_transComp.transform.position = glm::vec3(0.0f, 0.0f, 5.0f);

        // Plane
        s_plane = scene->CreateEntity("Plane");
        auto& plane_transComp = s_plane.GetComponent<TransformComponent>();
        plane_transComp.transform.position = glm::vec3(0.0f, -0.5f, 0.0f);
        plane_transComp.transform.scale = glm::vec3(10.0f, 0.1f, 10.0f);
        s_plane.AddComponent<MeshComponent>();

        // Cube
        s_cube = scene->CreateEntity("Cube");
        auto& cube_transComp = s_cube.GetComponent<TransformComponent>();
        cube_transComp.transform.position = glm::vec3(1.0f, 0.0f, 0.0f);
        s_cube.AddComponent<MeshComponent>();

        // Sphere
        s_sphere = scene->CreateEntity("Sphere");
        auto& sphere_transComp = s_sphere.GetComponent<TransformComponent>();
        sphere_transComp.transform.position = glm::vec3(-1.0f, 0.0f, 0.0f);
        s_sphere.AddComponent<MeshComponent>();

        scene->OnSceneStart();
    }
    void SandboxLayer::OnUpdate(float deltaTime)
    {
        if (cursorDisabled && Input::IsKeyDown(Escape))
        {
            Application::Get().GetWindow()->SetCursorMode(CursorMode::Normal);
            cursorDisabled = false;
        }
        else if (!cursorDisabled && Input::IsMouseButtonDown(MouseCode::Button0))
        {
            Application::Get().GetWindow()->SetCursorMode(CursorMode::Disabled);
            cursorDisabled = true;
        }

        if (cursorDisabled)
            ControlCamera(deltaTime);   

        Renderer::BeginDefaultRenderPass();

        //Renderer::RenderScene(SceneManager::GetActiveScene());
        s_rayShader->Bind();
        RenderCommand::DrawIndexed(s_vertexArray);

        Renderer::EndDefaultRenderPass();
    }
    void SandboxLayer::OnAPIHasInitialized(ApiType api)
    {
        // Fullscreen quad
        std::vector<float> vertices = {
             1.0f,  1.0f,  // top right
             1.0f, -1.0f,  // bottom right
            -1.0f, -1.0f,  // bottom left
            -1.0f,  1.0f,  // top left
        };
        std::vector<uint32_t> indices = {
            0, 1, 3,
            1, 2, 3
        };

        Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(vertices);
        BufferLayout layout = {
            { "inPos", ShaderDataType::FLOAT2, false }
        };
        vertexBuffer->SetLayout(layout);

        s_vertexArray = VertexArray::Create();
        s_vertexArray->AddVertexBuffer(vertexBuffer);
        s_vertexArray->SetIndexBuffer(IndexBuffer::Create(indices));

        // Ray tracing shader
        s_rayShader = Shader::Create("Assets/Shaders/RayTracing.glsl");

        // Load assets
        s_shader3D = Shader::Create("Assets/Shaders/3D.glsl");

        s_planeMaterial = Material::Create(s_shader3D);
        glm::vec4 planeColor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
        s_planeMaterial->SetConstant("uFragConst", "Color", &planeColor, sizeof(glm::vec4));

        s_cubeMesh = Mesh::Create("Assets/Meshes/cube.obj");
        s_cubeMaterial = Material::Create(s_shader3D);
        glm::vec4 cubeColor = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
        s_cubeMaterial->SetConstant("uFragConst", "Color", &cubeColor, sizeof(glm::vec4));

        s_sphereMesh = Mesh::Create("Assets/Meshes/sphere.obj");
        s_sphereMaterial = Material::Create(s_shader3D);
        glm::vec4 sphereColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
        s_sphereMaterial->SetConstant("uFragConst", "Color", &sphereColor, sizeof(glm::vec4));

        // Apply new assets to objects on the scene
        // Plane
        auto& planeMeshComp = SceneManager::GetActiveScene()->GetEntity(s_plane.GetUUID()).GetComponent<MeshComponent>();
        planeMeshComp.mesh = s_cubeMesh.get();
        planeMeshComp.material = s_planeMaterial.get();
        // Cube
        auto& cubeMeshComp = SceneManager::GetActiveScene()->GetEntity(s_cube.GetUUID()).GetComponent<MeshComponent>();
        cubeMeshComp.mesh = s_cubeMesh.get();
        cubeMeshComp.material = s_cubeMaterial.get();
        // Sphere
        auto& sphereMeshComp = SceneManager::GetActiveScene()->GetEntity(s_sphere.GetUUID()).GetComponent<MeshComponent>();
        sphereMeshComp.mesh = s_sphereMesh.get();
        sphereMeshComp.material = s_sphereMaterial.get();

        // Maximize the window
        Application::Get().GetWindow()->Maximize();

        // Lock the cursor
        Application::Get().GetWindow()->SetCursorMode(CursorMode::Disabled);
        cursorDisabled = true;
    }
    void SandboxLayer::OnShutdownAPI()
    {
        // Dispose fullscreen quad
        s_vertexArray->Dispose();
        s_rayShader->Dispose();

        // Dispose assets
        s_shader3D->Dispose();

        s_planeMaterial->Dispose();

        s_cubeMesh->Dispose();
        s_cubeMaterial->Dispose();

        s_sphereMesh->Dispose();
        s_sphereMaterial->Dispose();
    }
}