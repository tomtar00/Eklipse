#include <precompiled.h>
#include "RendererContext.h"
#include <Eklipse/Scene/Components.h>

namespace Eklipse
{
    // === Rasterization ===
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
    static std::pair<int, int> AddMeshToBuffers(const Mesh* mesh, uint32_t vertOffset, uint32_t indexOffset, uint32_t meshIndex)
    {
        Vec<float> vertices = mesh->GetVertices();
        auto buffer = Renderer::GetStorageBuffer("bVertices");
        buffer->SetData(vertices.data(), vertices.size() * sizeof(float), vertOffset * sizeof(float));

        Vec<uint32_t> indices = mesh->GetIndices();
        buffer = Renderer::GetStorageBuffer("bIndices");
        buffer->SetData(indices.data(), indices.size() * sizeof(uint32_t), indexOffset * sizeof(uint32_t));

        Bounds bounds = mesh->GetBounds();

        RayTracingMeshInfo meshInfo{};
        meshInfo.vertexOffset = vertOffset;
        meshInfo.vertexCount = vertices.size();
        meshInfo.indexOffset = indexOffset;
        meshInfo.indexCount = indices.size();
        meshInfo.boundMin = bounds.min;
        meshInfo.boundMax = bounds.max;
        meshInfo.materialIndex = meshIndex;

        uint32_t newMeshCount = meshIndex + 1;
        buffer = Renderer::GetStorageBuffer("bMeshes");
        buffer->SetData(&newMeshCount, sizeof(uint32_t));
        buffer->SetData(&meshInfo, sizeof(RayTracingMeshInfo), 4 * sizeof(uint32_t) + meshIndex * sizeof(RayTracingMeshInfo));

        return { vertices.size(), indices.size() };
    }

    void RayTracingContext::Init()
    {
        m_viewportSize = { Application::Get().GetInfo().windowWidth, Application::Get().GetInfo().windowHeight };
        m_lastViewportSize = m_viewportSize;

        InitSSBOs();

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

            m_fullscreenQuad = VertexArray::Create();
            m_fullscreenQuad->AddVertexBuffer(vertexBuffer);
            m_fullscreenQuad->SetIndexBuffer(IndexBuffer::Create(indices));
        }

        // Shader
        {
            m_shader = Shader::Create("Assets/Shaders/RT/RayTracing.glsl");
        }

        InitMaterial();

        m_transComputeShader = ComputeShader::Create("Assets/Shaders/RT/Transform.comp");

        ReconstructSceneBuffers();
    }
    void RayTracingContext::Shutdown()
    {
        m_fullscreenQuad->Dispose();
        m_shader->Dispose();
        m_material->Dispose();

        m_transComputeShader->Dispose();
    }
    void RayTracingContext::InitSSBOs()
    {
        const uint32_t maxVerticies = 1000000;
        const uint32_t maxIndices = 1000000;
        const uint32_t maxMeshes = 100;
        const uint32_t maxSpheres = 100;
        glm::vec2 screenSize = { Application::Get().GetInfo().windowWidth, Application::Get().GetInfo().windowHeight };
        size_t bufferSize = screenSize.x * screenSize.y * 4 * sizeof(float);
        Renderer::CreateStorageBuffer("bPixels", bufferSize, 1);
        Renderer::CreateStorageBuffer("bVertices", maxVerticies * sizeof(float), 2);
        Renderer::CreateStorageBuffer("bTransVertices", maxVerticies * sizeof(float), 3);
        Renderer::CreateStorageBuffer("bIndices", maxIndices * sizeof(uint32_t), 4);
        Renderer::CreateStorageBuffer("bSpheres", 4 * sizeof(uint32_t) + maxSpheres * sizeof(RayTracingSphereInfo), 5);
        Renderer::CreateStorageBuffer("bMeshes", 4 * sizeof(uint32_t) + maxMeshes * sizeof(RayTracingMeshInfo), 6);
        Renderer::CreateStorageBuffer("bMaterials", (maxMeshes + maxSpheres) * sizeof(RayTracingMaterial), 7);
        Renderer::CreateStorageBuffer("bTransforms", maxMeshes * sizeof(glm::mat4), 8);
    }
    void RayTracingContext::OnUpdate(float deltaTime)
    {
        Vec<RayTracingMaterial> materials(m_numTotalMeshes + m_numTotalSpheres);
        Vec<RayTracingSphereInfo> spheres(m_numTotalSpheres);
        Vec<glm::mat4> transforms(m_numTotalMeshes);

        auto scene = SceneManager::GetActiveScene();

        scene->GetRegistry().view<RayTracingMeshComponent>().each([&](auto entityID, RayTracingMeshComponent& rtComp)
        {
            Entity entity = { entityID, scene.get() };
            auto& transfomMatrix = entity.GetComponent<TransformComponent>().GetTransformMatrix(); // TODO: Optimize

            materials[rtComp.index] = rtComp.material;
            transforms[rtComp.index] = transfomMatrix;
        });
        int i = 0;
        scene->GetRegistry().view<RayTracingSphereComponent>().each([&](auto entityID, RayTracingSphereComponent& rtComp)
        {
            Entity entity = { entityID, scene.get() };

            RayTracingSphereInfo sphereInfo{};
            sphereInfo.position = entity.GetComponent<TransformComponent>().transform.position;
            sphereInfo.radius = rtComp.radius;
            sphereInfo.materialIndex = m_numTotalMeshes + i;

            materials[sphereInfo.materialIndex] = rtComp.material;
            spheres[i] = sphereInfo;
            ++i;
        });

        if (m_numTotalMeshes || m_numTotalSpheres)
        {
            auto buffer = Renderer::GetStorageBuffer("bMaterials");
            buffer->SetData(materials.data(), materials.size() * sizeof(RayTracingMaterial), 0);
        }
        if (m_numTotalMeshes)
        {
            auto buffer = Renderer::GetStorageBuffer("bTransforms");
            buffer->SetData(transforms.data(), transforms.size() * sizeof(glm::mat4), 0);
        }
        if (m_numTotalSpheres)
        {
            auto buffer = Renderer::GetStorageBuffer("bSpheres");
            buffer->SetData(&m_numTotalSpheres, sizeof(uint32_t));
            buffer->SetData(spheres.data(), spheres.size() * sizeof(RayTracingSphereInfo), 4 * sizeof(uint32_t));
        }

        if (m_lastViewportSize != m_viewportSize)
        {
            m_viewportSize = m_lastViewportSize;
            m_frameIndex = 0;

            Renderer::WaitDeviceIdle();

            m_material->Dispose();
            InitMaterial();
        }
    }
    void RayTracingContext::OnCompute(float deltaTime)
    {
        RenderCommand::Dispatch(m_transComputeShader, m_numTotalVertices / 3, 1, 1);
    }
    void RayTracingContext::OnWindowResize(uint32_t width, uint32_t height)
    {
        Renderer::WaitDeviceIdle();
        m_frameIndex = 0;

        m_material->Dispose();
        InitMaterial();
    }
    void RayTracingContext::OnMeshAdded(Entity entity)
    {
        auto& meshComp = entity.GetComponent<MeshComponent>();
        auto& rtMeshComp = entity.GetComponent<RayTracingMeshComponent>();
        rtMeshComp.index = m_numTotalMeshes;

        auto& sizes = AddMeshToBuffers(meshComp.mesh, m_numTotalVertices, m_numTotalIndices, m_numTotalMeshes++);

        m_numTotalVertices += sizes.first;
        m_numTotalIndices += sizes.second;
    }
    void RayTracingContext::OnSphereAdded(Entity entity)
    {
        m_numTotalSpheres++;
    }
    void RayTracingContext::OnSceneChanged()
    {
        Renderer::WaitDeviceIdle();
        ReconstructSceneBuffers();
    }
    void RayTracingContext::RenderScene(Ref<Scene> scene, Camera& camera, Transform& cameraTransform)
    {
        EK_PROFILE();

        ++m_frameIndex;

        m_material->SetConstant("pData", "CameraPos", &cameraTransform.position[0], sizeof(glm::vec3));
        m_material->SetConstant("pData", "Frames", &m_frameIndex, sizeof(int));

        Renderer::UpdateViewProjection(camera, cameraTransform);
        RenderCommand::DrawIndexed(m_fullscreenQuad, m_material.get());

        m_lastViewportSize = { g_currentFramebuffer->GetInfo().width, g_currentFramebuffer->GetInfo().height };
    }

    void RayTracingContext::SetAccumulate(bool accumulate)
    {
        uint32_t value = accumulate ? 1 : 0;
        m_material->SetConstant("pData", "Accumulate", &value, sizeof(uint32_t));
        m_frameIndex = 0;
    }
    void RayTracingContext::SetRaysPerPixel(uint32_t raysPerPixel)
    {
        m_material->SetConstant("pData", "RaysPerPixel", &raysPerPixel, sizeof(uint32_t));
    }
    void RayTracingContext::SetMaxBounces(uint32_t maxBounces)
    {
        m_material->SetConstant("pData", "MaxBounces", &maxBounces, sizeof(uint32_t));
    }

    void RayTracingContext::SetSkyColorHorizon(const glm::vec3& color)
    {
        m_material->SetConstant("pData", "SkyColorHorizon", &color, sizeof(glm::vec3));
    }
    void RayTracingContext::SetSkyColorZenith(const glm::vec3& color)
    {
        m_material->SetConstant("pData", "SkyColorZenith", &color, sizeof(glm::vec3));
    }
    void RayTracingContext::SetGroundColor(const glm::vec3& color)
    {
        m_material->SetConstant("pData", "GroundColor", &color, sizeof(glm::vec3));
    }
    void RayTracingContext::SetSunColor(const glm::vec3& color)
    {
        m_material->SetConstant("pData", "SunColor", &color, sizeof(glm::vec3));
    }
    void RayTracingContext::SetSunDirection(const glm::vec3& direction)
    {
        m_material->SetConstant("pData", "SunDirection", &direction, sizeof(glm::vec3));
    }
    void RayTracingContext::SetSunFocus(float focus)
    {
        m_material->SetConstant("pData", "SunFocus", &focus, sizeof(float));
    }
    void RayTracingContext::SetSunIntensity(float intensity)
    {
        m_material->SetConstant("pData", "SunIntensity", &intensity, sizeof(float));
    }

    void RayTracingContext::RecompileShader()
    {
        Renderer::WaitDeviceIdle();
        m_frameIndex = 0;

        if (m_shader->Compile("Assets/Shaders/RT/RayTracing.glsl", true))
            m_material->OnShaderReloaded();
    }
    void RayTracingContext::RecompileTransformComputeShader()
    {
        Renderer::WaitDeviceIdle();
        m_frameIndex = 0;

        if (m_transComputeShader->GetShader()->Compile("Assets/Shaders/RT/Transform.comp", true))
            m_transComputeShader->GetMaterial()->OnShaderReloaded();
    }

    void RayTracingContext::InitMaterial()
    {
        size_t bufferSize = m_viewportSize.x * m_viewportSize.y * 4 * sizeof(float);
        Renderer::CreateStorageBuffer("bPixels", bufferSize, 1);

        m_material = Material::Create(m_shader);

        auto& settings = Renderer::GetSettings();

        m_material->SetConstant("pData", "Resolution", &m_viewportSize, sizeof(glm::vec2));
        m_material->SetConstant("pData", "RaysPerPixel", &settings.raysPerPixel, sizeof(uint32_t));
        m_material->SetConstant("pData", "MaxBounces", &settings.maxBounces, sizeof(uint32_t));
        m_material->SetConstant("pData", "Accumulate", &settings.accumulate, sizeof(uint32_t));

        m_material->SetConstant("pData", "SkyColorHorizon", &settings.skyColorHorizon, sizeof(glm::vec3));
        m_material->SetConstant("pData", "SkyColorZenith", &settings.skyColorZenith, sizeof(glm::vec3));
        m_material->SetConstant("pData", "GroundColor", &settings.groundColor, sizeof(glm::vec3));
        m_material->SetConstant("pData", "SunColor", &settings.sunColor, sizeof(glm::vec3));
        m_material->SetConstant("pData", "SunDirection", &settings.sunDirection, sizeof(glm::vec3));
        m_material->SetConstant("pData", "SunFocus", &settings.sunFocus, sizeof(float));
        m_material->SetConstant("pData", "SunIntensity", &settings.sunIntensity, sizeof(float));
    }
    void RayTracingContext::ReconstructSceneBuffers()
    {
        m_numTotalIndices = 0;
        m_numTotalVertices = 0;
        m_numTotalMeshes = 0;
        m_numTotalSpheres = 0;

        auto scene = SceneManager::GetActiveScene();
        scene->GetRegistry().view<RayTracingMeshComponent>().each([&](auto entityID, RayTracingMeshComponent& rtComp)
        {
            Entity entity = { entityID, scene.get() };
            OnMeshAdded(entity);
        });
        auto buffer = Renderer::GetStorageBuffer("bMeshes");
        buffer->SetData(&m_numTotalMeshes, sizeof(uint32_t));

        scene->GetRegistry().view<RayTracingSphereComponent>().each([&](auto entityID, RayTracingSphereComponent& rtComp)
        {
            Entity entity = { entityID, scene.get() };
            OnSphereAdded(entity);
        });
        buffer = Renderer::GetStorageBuffer("bSpheres");
        buffer->SetData(&m_numTotalSpheres, sizeof(uint32_t));
    }
}