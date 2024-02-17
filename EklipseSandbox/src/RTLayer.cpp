#include "RTLayer.h"

namespace Eklipse
{
    void RTLayer::OnAttach()
    {
        m_cameraTransform.position = { 0.0f, 2.0f, 10.0f };
        m_cameraTransform.rotation = { -5.0f, 0.0f, 0.0f };
        m_camera.m_fov = 45.0f;
    }

    void RTLayer::OnEvent(Event& event)
    {
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& e)
        {
            glm::vec2 screenSize = { e.GetWidth(), e.GetHeight() };
            m_rayMaterial->SetConstant("pushConstants", "uResolution", &screenSize, sizeof(glm::vec2));
        }, false);
    }
    void RTLayer::OnGUI(float deltaTime)
    {
        ImGui::Begin("Ray Tracing");
        ImGui::Text("FPS: %f", Stats::Get().fps);
        if (ImGui::Button("Recompile Shader"))
        {
            if (m_rayShader->Compile("Assets/Shaders/RayTracing.glsl", true))
                m_rayMaterial->OnShaderReloaded();
        }
        ImGui::DragFloat3("Camera Position", &m_cameraTransform.position[0]);
        ImGui::DragFloat3("Camera Rotation", &m_cameraTransform.rotation[0]);
        ImGui::SliderFloat("Camera FOV", &m_camera.m_fov, 1.0f, 120.0f);
        ImGui::End();
    }
    void RTLayer::OnRender()
    {
        m_camera.UpdateViewProjectionMatrix(m_cameraTransform, g_defaultFramebuffer->GetAspectRatio());
        //m_rayMaterial->SetConstant("pushConstants", "uCameraPos", &m_cameraTransform.position, sizeof(glm::vec3));
        
        Renderer::UpdateViewProjection(m_camera, m_cameraTransform);
        RenderCommand::DrawIndexed(m_fullscreenVA, m_rayMaterial.get());
    }
    
    void RTLayer::OnAPIHasInitialized(ApiType api)
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

        m_fullscreenVA = VertexArray::Create();
        m_fullscreenVA->AddVertexBuffer(vertexBuffer);
        m_fullscreenVA->SetIndexBuffer(IndexBuffer::Create(indices));

        m_rayShader = Shader::Create("Assets/Shaders/RayTracing.glsl");
        m_rayMaterial = Material::Create(m_rayShader);

        glm::vec2 screenSize = { Application::Get().GetInfo().windowWidth, Application::Get().GetInfo().windowHeight };
        m_rayMaterial->SetConstant("pushConstants", "uResolution", &screenSize, sizeof(glm::vec2));
    }
    void RTLayer::OnShutdownAPI(bool quit)
    {
        m_fullscreenVA->Dispose();
        m_rayShader->Dispose();
        m_rayMaterial->Dispose();
    }
}