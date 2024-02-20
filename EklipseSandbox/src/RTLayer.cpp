#include "RTLayer.h"

namespace Eklipse
{
    void RTLayer::OnAttach()
    {
        m_cameraTransform.position = { 0.0f, 0.5f, 5.0f };
        m_cameraTransform.rotation = { -1.0f, 0.0f, 0.0f };
        m_camera.m_fov = 50.0f;

        m_shaderPath = "Assets/Shaders/RT_accum.glsl";

        m_frames = 1;
        m_raysPerPixel = 1;
        m_maxBounces = 3;
        m_reset = 0;

        // Background
        m_skyColorHorizon = { 0.7f, 0.9f, 1.0f };
        m_skyColorZenith = { 0.2f, 0.5f, 0.8f };
        m_groundColor = { 0.6f, 0.6f, 0.6f };
        m_sunColor = { 1.0f, 1.0f, 0.8f };
        m_sunDirection = { 0.0f, -0.5f, -1.0f };
        m_sunFocus = 0.99f;
        m_sunIntensity = 5.0f;
    }

    void RTLayer::OnEvent(Event& event)
    {
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& e)
        {
            Renderer::WaitDeviceIdle();
            ResetPixelBuffer();
            m_rayMaterial->Dispose();
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

        ImGui::Separator();
        static int shaderIndex = 1;
        if (ImGui::Combo("Shader", &shaderIndex, "RT_basic\0RT_accum"))
        {
            Renderer::WaitDeviceIdle();
            m_rayShader->Dispose();
            m_rayMaterial->Dispose();
            ResetPixelBuffer();
            switch (shaderIndex)
            {
                case 0: m_shaderPath = "Assets/Shaders/RT_basic.glsl"; break;
                case 1: m_shaderPath = "Assets/Shaders/RT_accum.glsl"; break;
            }
            InitShader();
            InitMaterial();
        }
        if (ImGui::Button("Recompile Shader"))
        {
            Renderer::WaitDeviceIdle();
            ResetPixelBuffer();
            if (m_rayShader->Compile(m_shaderPath, true))
                m_rayMaterial->OnShaderReloaded();
        }
        if (ImGui::SliderInt("Rays Per Pixel", &m_raysPerPixel, 1, 20))
        {
            m_rayMaterial->SetConstant("pData", "RaysPerPixel", &m_raysPerPixel, sizeof(int));
            needsReset = true;
        }
        if (ImGui::SliderInt("Max Bounces", &m_maxBounces, 1, 20))
        {
            m_rayMaterial->SetConstant("pData", "MaxBounces", &m_maxBounces, sizeof(int));
            needsReset = true;
        }

        ImGui::Separator();
        if (ImGui::ColorEdit3("Sky Color Horizon", &m_skyColorHorizon[0]))
        {
            m_rayMaterial->SetConstant("pData", "SkyColorHorizon", &m_skyColorHorizon, sizeof(glm::vec3));
            needsReset = true;
        }
        if (ImGui::ColorEdit3("Sky Color Zenith", &m_skyColorZenith[0]))
        {
            m_rayMaterial->SetConstant("pData", "SkyColorZenith", &m_skyColorZenith, sizeof(glm::vec3));
            needsReset = true;
        }
        if (ImGui::ColorEdit3("Ground Color", &m_groundColor[0]))
        {
            m_rayMaterial->SetConstant("pData", "GroundColor", &m_groundColor, sizeof(glm::vec3));
            needsReset = true;
        }
        if (ImGui::ColorEdit3("Sun Color", &m_sunColor[0]))
        {
            m_rayMaterial->SetConstant("pData", "SunColor", &m_sunColor, sizeof(glm::vec3));
            needsReset = true;
        }
        if (ImGui::DragFloat3("Sun Direction", &m_sunDirection[0], 0.1f))
        {
            m_rayMaterial->SetConstant("pData", "SunDirection", &m_sunDirection, sizeof(glm::vec3));
            needsReset = true;
        }
        if (ImGui::SliderFloat("Sun Focus", &m_sunFocus, 0.1f, 0.99f))
        {
            m_rayMaterial->SetConstant("pData", "SunFocus", &m_sunFocus, sizeof(float));
            needsReset = true;
        }
        if (ImGui::SliderFloat("Sun Intensity", &m_sunIntensity, 0.1f, 10.0f))
        {
            m_rayMaterial->SetConstant("pData", "SunIntensity", &m_sunIntensity, sizeof(float));
            needsReset = true;
        }
        
        if (needsReset)
        {
            ResetPixelBuffer();
        }
        ImGui::End();
    }
    void RTLayer::OnRender(float deltaTime)
    {
        m_camera.UpdateViewProjectionMatrix(m_cameraTransform, g_defaultFramebuffer->GetAspectRatio());
        m_rayMaterial->SetConstant("pData", "CameraPos", &m_cameraTransform.position[0], sizeof(glm::vec3));
        m_rayMaterial->SetConstant("pData", "Frames", &m_frames, sizeof(int));
        m_rayMaterial->SetConstant("pData", "Reset", &m_reset, sizeof(int));
        
        Renderer::UpdateViewProjection(m_camera, m_cameraTransform);
        RenderCommand::DrawIndexed(m_fullscreenVA, m_rayMaterial.get());

        m_reset = 0;
        ++m_frames;
    }
    
    void RTLayer::OnAPIHasInitialized(ApiType api)
    {
        InitQuad();
        InitShader();
        InitMaterial();
    }
    void RTLayer::OnShutdownAPI(bool quit)
    {
        m_fullscreenVA->Dispose();
        m_rayShader->Dispose();
        m_rayMaterial->Dispose();
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
        m_rayShader = Shader::Create(m_shaderPath);
    }
    void RTLayer::InitMaterial()
    {
        glm::vec2 screenSize = { Application::Get().GetInfo().windowWidth, Application::Get().GetInfo().windowHeight };

        size_t bufferSize = screenSize.x * screenSize.y * 4 * sizeof(float);
        m_pixelBuffer = Renderer::CreateStorageBuffer("bPixels", bufferSize, 1);
        m_meshBuffer = Renderer::CreateStorageBuffer("bMeshes", 1, 2);

        m_rayMaterial = Material::Create(m_rayShader);

        m_rayMaterial->SetConstant("pData", "Resolution", &screenSize, sizeof(glm::vec2));
        m_rayMaterial->SetConstant("pData", "RaysPerPixel", &m_raysPerPixel, sizeof(int));
        m_rayMaterial->SetConstant("pData", "MaxBounces", &m_maxBounces, sizeof(int));
        m_rayMaterial->SetConstant("pData", "Reset", &m_reset, sizeof(int));

        m_rayMaterial->SetConstant("pData", "SkyColorHorizon", &m_skyColorHorizon, sizeof(glm::vec3));
        m_rayMaterial->SetConstant("pData", "SkyColorZenith", &m_skyColorZenith, sizeof(glm::vec3));
        m_rayMaterial->SetConstant("pData", "GroundColor", &m_groundColor, sizeof(glm::vec3));
        m_rayMaterial->SetConstant("pData", "SunColor", &m_sunColor, sizeof(glm::vec3));
        m_rayMaterial->SetConstant("pData", "SunDirection", &m_sunDirection, sizeof(glm::vec3));
        m_rayMaterial->SetConstant("pData", "SunFocus", &m_sunFocus, sizeof(float));
        m_rayMaterial->SetConstant("pData", "SunIntensity", &m_sunIntensity, sizeof(float));

    }
    void RTLayer::ResetPixelBuffer()
    {
        m_frames = 1;
        m_reset = 1;
    }
}