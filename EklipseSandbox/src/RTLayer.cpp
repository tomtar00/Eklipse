#include "RTLayer.h"

namespace Eklipse
{
    void RTLayer::OnAttach()
    {
        m_cameraTransform.position = { 0.0f, 0.5f, 5.0f };
        m_cameraTransform.rotation = { -1.0f, 0.0f, 0.0f };
        m_camera.m_fov = 50.0f;

        m_shaderPath = "Assets/Shaders/RT/RT_mesh.glsl";

        m_frames        = 0;
        m_raysPerPixel  = 1;
        m_maxBounces    = 4;

        m_skyColorHorizon   = { 1.0f, 1.0f, 1.0f };
        m_skyColorZenith    = { 0.07f, 0.36f, 0.72f };  
        m_groundColor       = { 0.35f, 0.3f, 0.35f };
        m_sunColor          = { 1.0f, 1.0f, 0.8f };
        m_sunDirection      = { 0.0f, 0.3f, -1.0f };
        m_sunFocus          = 500.0f;
        m_sunIntensity      = 200.0f;

        m_cameraSpeed       = 3.0f;
        m_cameraSensitivity = 0.04f;
        m_cursorDisabled    = false;
    }

    void RTLayer::OnEvent(Event& event)
    {
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& e)
        {
            Renderer::WaitDeviceIdle();
            ResetPixelBuffer();
            m_material->Dispose();
            InitMaterial();
        }, false);
    }
    void RTLayer::OnGUI(float deltaTime)
    {
        ImGui::Begin("Ray Tracing");
        ImGui::Text("FPS: %f", Stats::Get().fps);
        
        ImGui::Separator();
        bool needsReset = false;
        needsReset |= ImGui::DragFloat3("Camera Position", &m_cameraTransform.position[0], 0.1f);
        needsReset |= ImGui::DragFloat3("Camera Rotation", &m_cameraTransform.rotation[0], 0.1f);
        needsReset |= ImGui::SliderFloat("Camera FOV", &m_camera.m_fov, 1.0f, 120.0f);
        ImGui::Checkbox("Control Camera", &m_controlCamera);
        if (m_controlCamera)
        {
            ImGui::DragFloat("Camera Speed", &m_cameraSpeed, 0.1f);
            ImGui::DragFloat("Camera Sensitivity", &m_cameraSensitivity, 0.001f);
        }
        ImGui::Separator();
        static int api = (int)Renderer::GetGraphicsAPIType();
        if (ImGui::Combo("Graphics API", &api, "Vulkan\0OpenGL"))
        {
            Renderer::WaitDeviceIdle();
            ResetPixelBuffer();
            switch (api)
            {
                case 0: Application::Get().SetGraphicsAPIType(GraphicsAPI::Type::Vulkan); break;
                case 1: Application::Get().SetGraphicsAPIType(GraphicsAPI::Type::OpenGL); break;
            }
        }
        ImGui::Separator();
        static int shaderIndex = 2;
        if (ImGui::Combo("Shader", &shaderIndex, "RT_basic\0RT_accum\0RT_mesh"))
        {
            Renderer::WaitDeviceIdle();
            m_shaderHandle->Dispose();
            m_material->Dispose();
            ResetPixelBuffer();
            switch (shaderIndex)
            {
                case 0: m_shaderPath = "Assets/Shaders/RT/RT_basic.glsl"; break;
                case 1: m_shaderPath = "Assets/Shaders/RT/RT_accum.glsl"; break;
                case 2: m_shaderPath = "Assets/Shaders/RT/RT_mesh.glsl"; break;
            }
            InitShader();
            InitMaterial();
        }
        if (ImGui::Button("Recompile Shader"))
        {
            Renderer::WaitDeviceIdle();
            ResetPixelBuffer();
            if (m_shaderHandle->Compile(m_shaderPath, true))
                m_material->OnShaderReloaded();
        }
        if (ImGui::SliderInt("Rays Per Pixel", &m_raysPerPixel, 1, 20))
        {
            m_material->SetConstant("pData", "RaysPerPixel", &m_raysPerPixel, sizeof(int));
            needsReset = true;
        }
        if (ImGui::SliderInt("Max Bounces", &m_maxBounces, 1, 20))
        {
            m_material->SetConstant("pData", "MaxBounces", &m_maxBounces, sizeof(int));
            needsReset = true;
        }

        ImGui::Separator();
        if (ImGui::ColorEdit3("Sky Color Horizon", &m_skyColorHorizon[0]))
        {
            m_material->SetConstant("pData", "SkyColorHorizon", &m_skyColorHorizon, sizeof(glm::vec3));
            needsReset = true;
        }
        if (ImGui::ColorEdit3("Sky Color Zenith", &m_skyColorZenith[0]))
        {
            m_material->SetConstant("pData", "SkyColorZenith", &m_skyColorZenith, sizeof(glm::vec3));
            needsReset = true;
        }
        if (ImGui::ColorEdit3("Ground Color", &m_groundColor[0]))
        {
            m_material->SetConstant("pData", "GroundColor", &m_groundColor, sizeof(glm::vec3));
            needsReset = true;
        }
        if (ImGui::ColorEdit3("Sun Color", &m_sunColor[0]))
        {
            m_material->SetConstant("pData", "SunColor", &m_sunColor, sizeof(glm::vec3));
            needsReset = true;
        }
        if (ImGui::DragFloat3("Sun Direction", &m_sunDirection[0], 0.1f))
        {
            m_material->SetConstant("pData", "SunDirection", &m_sunDirection, sizeof(glm::vec3));
            needsReset = true;
        }
        if (ImGui::SliderFloat("Sun Focus", &m_sunFocus, 0.1f, 1000.0f))
        {
            m_material->SetConstant("pData", "SunFocus", &m_sunFocus, sizeof(float));
            needsReset = true;
        }
        if (ImGui::SliderFloat("Sun Intensity", &m_sunIntensity, 0.1f, 1000.0f))
        {
            m_material->SetConstant("pData", "SunIntensity", &m_sunIntensity, sizeof(float));
            needsReset = true;
        }
        
        if (needsReset)
        {
            ResetPixelBuffer();
        }

        if (m_controlCamera)
        {
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
        }

        ImGui::End();
    }
    void RTLayer::OnRender(float deltaTime)
    {
        ++m_frames;

        m_material->SetConstant("pData", "CameraPos", &m_cameraTransform.position[0], sizeof(glm::vec3));
        m_material->SetConstant("pData", "Frames", &m_frames, sizeof(int));
        
        Renderer::UpdateViewProjection(m_camera, m_cameraTransform);
        RenderCommand::DrawIndexed(m_fullscreenVA, m_material.get());
    }
    void RTLayer::OnUpdate(float deltaTime)
    {
        if (m_controlCamera && m_cursorDisabled)
            ControlCamera(deltaTime);
    }
    
    void RTLayer::OnAPIHasInitialized(GraphicsAPI::Type api)
    {
        InitQuad();
        InitShader();
        InitMeshes();
        InitMaterial();
    }
    void RTLayer::OnShutdownAPI(bool quit)
    {
        m_fullscreenVA->Dispose();
        m_shaderHandle->Dispose();
        m_material->Dispose();

        m_cubeMesh->Dispose();
    }

    void RTLayer::InitQuad()
    {
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

        m_fullscreenVA = VertexArray::Create();
        m_fullscreenVA->AddVertexBuffer(vertexBuffer);
        m_fullscreenVA->SetIndexBuffer(IndexBuffer::Create(indices));
    }
    void RTLayer::InitShader()
    {
        m_shaderHandle = Shader::Create(m_shaderPath);
    }
    void RTLayer::InitMaterial()
    {
        glm::vec2 screenSize = { Application::Get().GetInfo().windowWidth, Application::Get().GetInfo().windowHeight };

        size_t bufferSize = screenSize.x * screenSize.y * 4 * sizeof(float);
        Renderer::CreateStorageBuffer("bPixels", bufferSize, 1);

        if (m_shaderPath == "Assets/Shaders/RT/RT_mesh.glsl")
        {
            /*
            RTMaterial material{};
            material.albedo = { 1.0f, 0.0f, 0.0f };
            material.smoothness = 0.0f;
            material.specularProb = 0.0f;
            material.specularColor = { 0.0f, 0.0f, 0.0f };
            material.emissionColor = { 0.0f, 0.0f, 0.0f };
            material.emissionStrength = 0.0f;

            MeshInfo meshInfo{};
            meshInfo.firstTriangle = 0;
            meshInfo.numTriangles = 1;
            meshInfo.boundMin = {};
            meshInfo.boundMax = {};
            meshInfo.material = material;

            Triangle triangle{};
            triangle.a = {  1.0f, 0.0f, 0.0f };
            triangle.b = {  0.0f, 1.0f, 0.0f };
            triangle.c = {  0.0f, 0.0f, 1.0f };

            auto trBuff = Renderer::CreateStorageBuffer("bTriangles", meshInfo.numTriangles * sizeof(Triangle), 2);
            trBuff->SetData(&triangle, meshInfo.numTriangles * sizeof(Triangle));

            Meshes meshes{};
            meshes.numMeshes = 1;
            meshes.meshes = &meshInfo;

            auto mshBuff = Renderer::CreateStorageBuffer("bMeshes", 4 * sizeof(uint32_t) + meshes.numMeshes * sizeof(MeshInfo), 3);
            mshBuff->SetData(&meshes.numMeshes, sizeof(uint32_t));
            mshBuff->SetData(meshes.meshes, meshes.numMeshes * sizeof(MeshInfo), 4 * sizeof(uint32_t));
            */
            
            RTMaterial material{};
            material.albedo = { 1.0f, 0.0f, 0.0f };
            material.smoothness = 0.0f;
            material.specularProb = 0.0f;
            material.specularColor = { 0.0f, 0.0f, 0.0f };
            material.emissionColor = { 0.0f, 0.0f, 0.0f };
            material.emissionStrength = 0.0f;

            Vec<Triangle> triangles = m_cubeMesh->GetTriangles();
            MeshInfo meshInfo{};
            Bounds bounds = m_cubeMesh->GetBounds();
            meshInfo.firstTriangle = 0;
            meshInfo.numTriangles = triangles.size();
            meshInfo.boundMin = bounds.min;
            meshInfo.boundMax = bounds.max;
            meshInfo.material = material;

            auto trBuff = Renderer::CreateStorageBuffer("bTriangles", meshInfo.numTriangles * sizeof(Triangle), 2);
            trBuff->SetData(triangles.data(), meshInfo.numTriangles * sizeof(Triangle));

            Meshes meshes{};
            meshes.numMeshes = 1;
            meshes.meshes = &meshInfo;

            auto mshBuff = Renderer::CreateStorageBuffer("bMeshes", 4 * sizeof(uint32_t) + meshes.numMeshes * sizeof(MeshInfo), 3);
            mshBuff->SetData(&meshes.numMeshes, sizeof(uint32_t));
            mshBuff->SetData(meshes.meshes, meshes.numMeshes * sizeof(MeshInfo), 4 * sizeof(uint32_t));
        }

        m_material = Material::Create(m_shaderHandle);
        
        m_material->SetConstant("pData", "Resolution", &screenSize, sizeof(glm::vec2));
        m_material->SetConstant("pData", "RaysPerPixel", &m_raysPerPixel, sizeof(int));
        m_material->SetConstant("pData", "MaxBounces", &m_maxBounces, sizeof(int));
        
        m_material->SetConstant("pData", "SkyColorHorizon", &m_skyColorHorizon, sizeof(glm::vec3));
        m_material->SetConstant("pData", "SkyColorZenith", &m_skyColorZenith, sizeof(glm::vec3));
        m_material->SetConstant("pData", "GroundColor", &m_groundColor, sizeof(glm::vec3));
        m_material->SetConstant("pData", "SunColor", &m_sunColor, sizeof(glm::vec3));
        m_material->SetConstant("pData", "SunDirection", &m_sunDirection, sizeof(glm::vec3));
        m_material->SetConstant("pData", "SunFocus", &m_sunFocus, sizeof(float));
        m_material->SetConstant("pData", "SunIntensity", &m_sunIntensity, sizeof(float));
    }
    void RTLayer::InitMeshes()
    {
        m_cubeMesh = Mesh::Create("Assets/Meshes/cube.obj");
    }
    void RTLayer::ResetPixelBuffer()
    {
        m_frames = 0;
    }

    void RTLayer::ControlCamera(float deltaTime)
    {
        bool needsReset = false;

        if (Input::IsKeyDown(W))
        {
            m_cameraTransform.position += m_cameraTransform.GetForward() * m_cameraSpeed * deltaTime;
            needsReset = true;
        }
        if (Input::IsKeyDown(S))
        {
            m_cameraTransform.position -= m_cameraTransform.GetForward() * m_cameraSpeed * deltaTime;
            needsReset = true;
        }
        if (Input::IsKeyDown(A))
        {
            m_cameraTransform.position -= m_cameraTransform.GetRight() * m_cameraSpeed * deltaTime;
            needsReset = true;
        }
        if (Input::IsKeyDown(D))
        {
            m_cameraTransform.position += m_cameraTransform.GetRight() * m_cameraSpeed * deltaTime;
            needsReset = true;
        }
        if (Input::IsKeyDown(LeftShift))
        {
            m_cameraTransform.position -= m_cameraTransform.GetUp() * m_cameraSpeed * deltaTime;
            needsReset = true;
        }
        if (Input::IsKeyDown(Space))
        {
            m_cameraTransform.position += m_cameraTransform.GetUp() * m_cameraSpeed * deltaTime;
            needsReset = true;
        }

        auto mouseDelta = Input::GetMouseDelta();
        if (mouseDelta.x != 0 || mouseDelta.y != 0)
        {
            m_cameraTransform.rotation.y -= mouseDelta.x * m_cameraSensitivity;
            m_cameraTransform.rotation.x -= mouseDelta.y * m_cameraSensitivity;
            needsReset = true;
        }

        if (needsReset)
        {
            ResetPixelBuffer();
        }
    }
}