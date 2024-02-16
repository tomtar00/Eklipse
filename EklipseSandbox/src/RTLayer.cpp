#include "RTLayer.h"

namespace Eklipse
{
    void RTLayer::OnAttach()
    {
    }
    void RTLayer::OnRender()
    {
        RenderCommand::DrawIndexed(m_fullscreenVA, m_rayMaterial.get());
    }
    void RTLayer::OnGUI(float deltaTime)
    {
        ImGui::Begin("Ray Tracing");
        ImGui::Text("Hello, world!");
        ImGui::End();
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

        glm::vec2 screenSize = { 1600.0f, 900.0f };
        m_rayMaterial->SetConstant("pushConstants", "uResolution", &screenSize, sizeof(glm::vec2));
    }
    void RTLayer::OnShutdownAPI(bool quit)
    {
        m_fullscreenVA->Dispose();
        m_rayShader->Dispose();
        m_rayMaterial->Dispose();
    }
}