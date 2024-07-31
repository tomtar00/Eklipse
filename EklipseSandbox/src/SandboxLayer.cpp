#include "SandboxLayer.h"
#include <Eklipse/Renderer/RendererContext.h>

namespace Eklipse
{
    void SandboxLayer::OnAttach()
    {
        m_cubeColor = glm::vec3(1.0f, 0.0f, 0.0f);
        m_teapotColor = glm::vec3(0.0f, 0.0f, 1.0f);
        m_cowColor = glm::vec3(0.0f, 1.0f, 0.0f);

        Renderer::GetSettings().skyColorHorizon = glm::vec3(1.0f, 1.0f, 1.0f);
        Renderer::GetSettings().skyColorZenith = glm::vec3(1.0f, 1.0f, 1.0f);
        Renderer::GetSettings().groundColor = glm::vec3(1.0f, 1.0f, 1.0f);
        Renderer::GetSettings().sunColor = glm::vec3(1.0f, 1.0f, 1.0f);
        Renderer::GetSettings().sunDirection = glm::vec3(0.0f, 0.3f, -1.0f);
        Renderer::GetSettings().sunFocus = 500.0f;
        Renderer::GetSettings().sunIntensity = 0.0f;
    }
    void SandboxLayer::OnUpdate(float deltaTime)
    {
        if (m_cursorDisabled)
            ControlCamera(deltaTime);
    }
    void SandboxLayer::OnRender(float deltaTime)
    {
        Renderer::RenderScene(SceneManager::GetActiveScene());

        if (m_drawBVH)
        {
            auto rtContext = std::static_pointer_cast<RayTracingContext>(Renderer::GetRendererContext());
            rtContext->DrawBVHNodes();
        }
    }
    void SandboxLayer::OnGUI(float deltaTime)
    {
        ImGui::Begin("Sandbox");

        if (!m_cursorDisabled)
        {
            if (ImGui::Button("Control camera"))
            {
                Application::Get().GetWindow()->SetCursorMode(CursorMode::Disabled);
                m_cursorDisabled = true;
            }
        }
        else
        {
            ImGui::Text("Press ESC to release the cursor");
        }

        ImGui::Text("FPS: %f", Stats::Get().fps);

        
        auto& transform = m_cameraEntity.GetComponent<TransformComponent>().transform;
        ImGui::Text("Camera pos: x=%f y=%f z=%f ms", transform.position.x, transform.position.y, transform.position.z);
        ImGui::Text("Camera rot: x=%f y=%f z=%f ms", transform.rotation.x, transform.rotation.y, transform.rotation.z);
        
        if (ImGui::Combo("Scene", &m_currentScene, "Tensor 2x2x2\0Tensor 3x3x3\0Tensor 4x4x4\0Tensor 5x5x5\0Tensor 6x6x6\0Tensor 7x7x7\0Tensor 8x8x8\0TeaCow\0Teapot"))
        {
            SwitchScene(m_currentScene);
        }
        ImGui::Separator();
        static int api = (int)Renderer::GetGraphicsAPIType();
        if (ImGui::Combo("Graphics API", &api, "Vulkan\0OpenGL"))
        {
            Renderer::WaitDeviceIdle();
            switch (api)
            {
                case 0: Application::Get().SetGraphicsAPIType(GraphicsAPI::Type::Vulkan); break;
                case 1: Application::Get().SetGraphicsAPIType(GraphicsAPI::Type::OpenGL); break;
            }
        }
        static int polygonMode = (int)Renderer::GetSettings().PipelineTopologyMode;
        if (ImGui::Combo("Topology mode", &polygonMode, "Triangle\0Line"))
        {
            switch (polygonMode)
            {
                case 0: Renderer::SetPipelineTopologyMode(Pipeline::TopologyMode::Triangle);    break;
                case 1: Renderer::SetPipelineTopologyMode(Pipeline::TopologyMode::Line);        break;
            }
        }
        ImGui::Separator();
        static int renderer = (int)Renderer::GetSettings().PipelineType;
        if (ImGui::Combo("Renderer", &renderer, "Raster\0RayTracing"))
        {
            switch (renderer)
            {
                case 0: Renderer::RequestPipelineTypeChange(Pipeline::Type::Rasterization);   break;
                case 1: Renderer::RequestPipelineTypeChange(Pipeline::Type::RayTracing);      break;
            }
        }
        if (Renderer::GetSettings().PipelineType == Pipeline::Type::RayTracing)
        {
            auto rtContext = std::static_pointer_cast<RayTracingContext>(Renderer::GetRendererContext());
            ImGui::Indent();

            if (ImGui::Button("Recompile Shader"))
            {
                rtContext->RecompileShader();
            }
            if (ImGui::Button("Recompile Transform Compute Shader"))
            {
                rtContext->RecompileTransformComputeShader();
            }

            ImGui::Spacing();
            if (ImGui::Checkbox("Use BVH", &Renderer::GetSettings().useBVH))
            {
                rtContext->SetUseBVH(Renderer::GetSettings().useBVH);
            }
            ImGui::Checkbox("Draw BVH", &m_drawBVH);
            if (ImGui::SliderInt("BVH Depth", &Renderer::GetSettings().maxBVHDepth, 1, 25))
            {
                rtContext->RebuildBVH();
            }
            if (ImGui::SliderInt("BVH Tris Per Leaf", &Renderer::GetSettings().maxTrianglesPerLeaf, 2, 100))
            {
                rtContext->RebuildBVH();
            }

            ImGui::Spacing();
            if (ImGui::Checkbox("Accumulate", &Renderer::GetSettings().accumulate))
            {
                rtContext->SetAccumulate(Renderer::GetSettings().accumulate);
            }
            if (ImGui::InputInt("Rays Per Pixel", &Renderer::GetSettings().raysPerPixel))
            {
                rtContext->SetRaysPerPixel(Renderer::GetSettings().raysPerPixel);
            }
            if (ImGui::InputInt("Max Bounces", &Renderer::GetSettings().maxBounces))
            {
                rtContext->SetMaxBounces(Renderer::GetSettings().maxBounces);
            }

            ImGui::SeparatorText("Sky");
            if (ImGui::ColorEdit3("Sky Color Horizon", &Renderer::GetSettings().skyColorHorizon[0]))
            {
                rtContext->SetSkyColorHorizon(Renderer::GetSettings().skyColorHorizon);
            }
            if (ImGui::ColorEdit3("Sky Color Zenith", &Renderer::GetSettings().skyColorZenith[0]))
            {
                rtContext->SetSkyColorZenith(Renderer::GetSettings().skyColorZenith);
            }
            if (ImGui::ColorEdit3("Ground Color", &Renderer::GetSettings().groundColor[0]))
            {
                rtContext->SetGroundColor(Renderer::GetSettings().groundColor);
            }
            if (ImGui::ColorEdit3("Sun Color", &Renderer::GetSettings().sunColor[0]))
            {
                rtContext->SetSunColor(Renderer::GetSettings().sunColor);
            }
            if (ImGui::DragFloat3("Sun Direction", &Renderer::GetSettings().sunDirection[0], 0.01f, -1.0f, 1.0f))
            {
                rtContext->SetSunDirection(Renderer::GetSettings().sunDirection);
            }
            if (ImGui::DragFloat("Sun Focus", &Renderer::GetSettings().sunFocus))
            {
                rtContext->SetSunFocus(Renderer::GetSettings().sunFocus);
            }
            if (ImGui::DragFloat("Sun Intensity", &Renderer::GetSettings().sunIntensity))
            {
                rtContext->SetSunIntensity(Renderer::GetSettings().sunIntensity);
            }

            ImGui::Unindent();
        }
        ImGui::Separator();

        //bool isHovered = ImGui::IsWindowHovered() || ImGui::IsAnyItemHovered();
        if (m_cursorDisabled && Input::IsKeyDown(Escape))
        {
            Application::Get().GetWindow()->SetCursorMode(CursorMode::Normal);
            m_cursorDisabled = false;
        }
        /*else if (!isHovered && !m_cursorDisabled && Input::IsMouseButtonDown(MouseCode::Button0))
        {
            Application::Get().GetWindow()->SetCursorMode(CursorMode::Disabled);
            m_cursorDisabled = true;
        }*/

        ImGui::End();
    }
    void SandboxLayer::OnAPIHasInitialized(GraphicsAPI::Type api)
    {
        // Geometry
        m_shader3D = Shader::Create("Assets/Shaders/3D.glsl");

        m_cubeMesh = Mesh::Create("Assets/Meshes/cube.obj");
        m_cubeMaterial = Material::Create(m_shader3D);
        m_cubeMaterial->SetConstant("uFragConst", "Color", &m_cubeColor, sizeof(glm::vec3));

        m_teapotMesh = Mesh::Create("Assets/Meshes/teapot.obj");
        m_teapotMaterial = Material::Create(m_shader3D);
        m_teapotMaterial->SetConstant("uFragConst", "Color", &m_teapotColor, sizeof(glm::vec3));

        m_cowMesh = Mesh::Create("Assets/Meshes/cow.obj");
        m_cowMaterial = Material::Create(m_shader3D);
        m_cowMaterial->SetConstant("uFragConst", "Color", &m_cowColor, sizeof(glm::vec3));

        SwitchScene(m_currentScene);
    }
    void SandboxLayer::OnShutdownAPI(bool quit)
    {
        // Dispose assets
        m_shader3D->Dispose();

        m_cubeMesh->Dispose();
        m_cubeMaterial->Dispose();

        m_teapotMesh->Dispose();
        m_teapotMaterial->Dispose();

        m_cowMesh->Dispose();
        m_cowMaterial->Dispose();
    }

    void SandboxLayer::CreateCamera(Ref<Scene> scene, float fov, glm::vec3& position, glm::vec3& rotation) 
    {
        m_cameraEntity = scene->CreateEntity("Camera");
        auto& cameraComp = m_cameraEntity.AddComponent<CameraComponent>();
        cameraComp.camera.m_isMain = true;
        cameraComp.camera.m_fov = fov;
        auto& camera_transComp = m_cameraEntity.GetComponent<TransformComponent>();
        camera_transComp.transform.position = position;
        camera_transComp.transform.rotation = rotation;
    }
    void SandboxLayer::CreateCube(Ref<Scene> scene, glm::vec3& position, glm::vec3& scale, glm::vec3& color, float smoothness, float specularProb) 
    {
        auto cube = scene->CreateEntity("Cube");
        auto& cube_transComp = cube.GetComponent<TransformComponent>();
        cube_transComp.transform.position = position;
        cube_transComp.transform.scale = scale;
        auto& cube_mesh = cube.AddComponent<MeshComponent>();
        cube_mesh.mesh = m_cubeMesh.get();
        cube_mesh.material = m_cubeMaterial.get();
        auto& rtCubeMesh = cube.AddComponent<RayTracingMeshComponent>();
        rtCubeMesh.material.albedo = color;
        rtCubeMesh.material.smoothness = smoothness;
        rtCubeMesh.material.specularProb = specularProb;
        rtCubeMesh.material.specularColor = glm::vec3(1.0f);
    }
    void SandboxLayer::CreateTeapot(Ref<Scene> scene, glm::vec3& position, glm::vec3& scale, glm::vec3& color, float smoothness, float specularProb)
    {
        auto teapot = scene->CreateEntity("Teapot");
        auto& teapot_transComp = teapot.GetComponent<TransformComponent>();
        teapot_transComp.transform.position = position;
        teapot_transComp.transform.scale = scale;
        auto& teapot_mesh = teapot.AddComponent<MeshComponent>();
        teapot_mesh.mesh = m_teapotMesh.get();
        teapot_mesh.material = m_teapotMaterial.get();
        auto& rtTeapotMesh = teapot.AddComponent<RayTracingMeshComponent>();
        rtTeapotMesh.material.albedo = color;
        rtTeapotMesh.material.smoothness = smoothness;
        rtTeapotMesh.material.specularProb = specularProb;
        rtTeapotMesh.material.specularColor = glm::vec3(1.0f);
    }
    void SandboxLayer::CreateCow(Ref<Scene> scene, glm::vec3& position, glm::vec3& scale, glm::vec3& color, float smoothness, float specularProb)
    {
        auto cow = scene->CreateEntity("Cow");
        auto& cow_transComp = cow.GetComponent<TransformComponent>();
        cow_transComp.transform.position = position;
        cow_transComp.transform.scale = scale;
        auto& cow_mesh = cow.AddComponent<MeshComponent>();
        cow_mesh.mesh = m_cowMesh.get();
        cow_mesh.material = m_cowMaterial.get();
        auto& rtCowMesh = cow.AddComponent<RayTracingMeshComponent>();
        rtCowMesh.material.albedo = color;
        rtCowMesh.material.smoothness = smoothness;
        rtCowMesh.material.specularProb = specularProb;
        rtCowMesh.material.specularColor = glm::vec3(1.0f);
    }

    Ref<Scene> SandboxLayer::SetupScene_CubeTensor(uint32_t dimension)
    {
        auto scene = CreateRef<Scene>();

        const float spacing = 0.5f;
        const float tensor_width = dimension + (dimension-1) * spacing;
        CreateCamera(scene, 45.0f, glm::vec3(-tensor_width, 2*tensor_width, 2*tensor_width), glm::vec3(-37.0f, -45.0f, 0.0f));

        for (int i = 0; i < dimension; i++) {
            for (int j = 0; j < dimension; j++) {
                for (int k = 0; k < dimension; k++) {
                    glm::vec3 position = glm::vec3(i + spacing * i, j + spacing * j, k + spacing * k);
                    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
                    glm::vec3 color = glm::vec3(i / (float)dimension, j / (float)dimension, k / (float)dimension);
                    CreateCube(scene, position, scale, color);
                }
            }
        }

        return scene;
    }    
    Ref<Scene> SandboxLayer::SetupScene_2()
    {
        auto scene = CreateRef<Scene>();

        CreateCamera(scene, 45.0f, glm::vec3(12.5f, 4.7f, -4.5f), glm::vec3(-7.3f, 112.5, 0.0f));

        CreateCube(scene, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(30.0f, 0.1f, 30.0f), glm::vec3(1.0f, 1.0f, 1.0f));
        CreateCube(scene, glm::vec3(0.0f, 0.0f, 4.0f), glm::vec3(30.0f, 10.0f, 0.1f), glm::vec3(1.0f, 1.0f, 1.0f));
        CreateCube(scene, glm::vec3(-4.0f, 0.0f, 0.0f), glm::vec3(0.1f, 10.0f, 30.0f), glm::vec3(1.0f, 1.0f, 1.0f));

        CreateTeapot(scene, glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), 0.9f, 0.5f);
        CreateCow(scene, glm::vec3(0.0f, 3.5f, -2.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        return scene;
    }
    Ref<Scene> SandboxLayer::SetupScene_3()
    {
        auto scene = CreateRef<Scene>();

        CreateCamera(scene, 45.0f, glm::vec3(4.8f, 5.2f, -6.15f), glm::vec3(-19.0f, 137.0f, 0.0f));

        CreateCube(scene, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(30.0f, 0.1f, 30.0f), glm::vec3(1.0f, 1.0f, 1.0f));
        CreateCube(scene, glm::vec3(0.0f, 0.0f, 4.0f), glm::vec3(30.0f, 10.0f, 0.1f), glm::vec3(1.0f, 1.0f, 1.0f)/*, 1.0f, 1.0f*/);
        CreateCube(scene, glm::vec3(-3.2f, 0.0f, 0.0f), glm::vec3(0.1f, 10.0f, 30.0f), glm::vec3(1.0f, 1.0f, 1.0f)/*, 1.0f, 1.0f*/);

        CreateTeapot(scene, glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), 0.9f, 0.5f);

        return scene;
    }

    void SandboxLayer::SwitchScene(Ref<Scene> scene)
    {
        SceneManager::SetActiveScene(scene);
        scene->OnSceneStart();
    }
    void SandboxLayer::SwitchScene(int sceneIndex)
    {
        switch (sceneIndex)
        {
            case 0: SwitchScene(SetupScene_CubeTensor(2)); break;
            case 1: SwitchScene(SetupScene_CubeTensor(3)); break;
            case 2: SwitchScene(SetupScene_CubeTensor(4)); break;
            case 3: SwitchScene(SetupScene_CubeTensor(5)); break;
            case 4: SwitchScene(SetupScene_CubeTensor(6)); break;
            case 5: SwitchScene(SetupScene_CubeTensor(7)); break;
            case 6: SwitchScene(SetupScene_CubeTensor(8)); break;
            case 7: SwitchScene(SetupScene_2()); break;
            case 8: SwitchScene(SetupScene_3()); break;
        }
    }
    void SandboxLayer::ControlCamera(float deltaTime)
    {
        auto& transComp = m_cameraEntity.GetComponent<TransformComponent>();
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