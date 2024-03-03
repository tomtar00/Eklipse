#include <precompiled.h>
#include "RendererContext.h"
#include <Eklipse/Scene/Components.h>

namespace Eklipse
{
    // === Rasterization ===
    void RasterizationContext::Init()
    {
    }
    void RasterizationContext::Shutdown()
    {
    }
    void RasterizationContext::RenderScene(Ref<Scene> scene, Camera& camera, Transform& cameraTransform)
    {
        EK_PROFILE();

        // Geometry
        auto view = scene->GetRegistry().view<TransformComponent, MeshComponent>();
        for (auto& entity : view)
        {
            auto [transformComponent, meshComponent] = view.get<TransformComponent, MeshComponent>(entity);

            if (meshComponent.mesh == nullptr || meshComponent.material == nullptr || !meshComponent.material->IsValid())
                continue;

            glm::mat4& modelMatrix = transformComponent.GetTransformMatrix();
            meshComponent.material->SetConstant("uVertConst", "Model", &modelMatrix[0][0], sizeof(glm::mat4));
            RenderCommand::DrawIndexed(meshComponent.mesh->GetVertexArray(), meshComponent.material);
        }

        // ...
    }

    // === Ray Tracing ===
    void RayTracingContext::Init()
    {
        // Quad
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

            m_rayTracingQuad = VertexArray::Create();
            m_rayTracingQuad->AddVertexBuffer(vertexBuffer);
            m_rayTracingQuad->SetIndexBuffer(IndexBuffer::Create(indices));
        }

        // Shader
        {
            m_rayTracingShader = Shader::Create("Assets/Shaders/RayTracing.glsl");
        }

        // Material
        {
            m_rayTracingMaterial = Material::Create(m_rayTracingShader);
        }
    }
    void RayTracingContext::Shutdown()
    {
        m_rayTracingQuad->Dispose();
        m_rayTracingShader->Dispose();
        m_rayTracingMaterial->Dispose();
    }
    void RayTracingContext::RenderScene(Ref<Scene> scene, Camera& camera, Transform& cameraTransform)
    {
        EK_PROFILE();

        // foreach mesh in the scene, take its pbr material and add it to the ray tracing pipeline

        ++m_frameIndex;

        m_rayTracingMaterial->SetConstant("pData", "CameraPos", &cameraTransform.position[0], sizeof(glm::vec3));
        m_rayTracingMaterial->SetConstant("pData", "Frames", &m_frameIndex, sizeof(int));

        Renderer::UpdateViewProjection(camera, cameraTransform);
        RenderCommand::DrawIndexed(m_rayTracingQuad, m_rayTracingMaterial.get());
    }
}