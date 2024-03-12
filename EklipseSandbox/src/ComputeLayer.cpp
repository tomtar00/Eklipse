#include "ComputeLayer.h"
#include <Eklipse/Renderer/RendererContext.h>

namespace Eklipse
{
    void ComputeLayer::OnAttach()
    {
        m_cameraTransform.position = { 0.0f, 0.5f, 5.0f };
        m_cameraTransform.rotation = { -1.0f, 0.0f, 0.0f };
        m_camera.m_fov = 50.0f;

        m_cameraSpeed = 3.0f;
        m_cameraSensitivity = 0.04f;
        m_cursorDisabled = false;

        m_scene = CreateRef<Scene>();
    }
    void ComputeLayer::OnEvent(Event& event)
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

    void ComputeLayer::OnGUI(float deltaTime)
    {
        ImGui::Begin("Ray Tracing");
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
            needsReset = true;
            switch (api)
            {
                case 0: Application::Get().SetGraphicsAPIType(GraphicsAPI::Type::Vulkan); break;
                case 1: Application::Get().SetGraphicsAPIType(GraphicsAPI::Type::OpenGL); break;
            }
        }
        ImGui::Separator();
        if (ImGui::Button("Recompile Shader"))
        {
            Renderer::WaitDeviceIdle();
            needsReset = true;
            if (m_rayShader->Compile("Assets/Shaders/Compute/RT_mesh.glsl", true))
                m_rayMaterial->OnShaderReloaded();
        }
        if (ImGui::Button("Recompile Compute Shader"))
        {
            Renderer::WaitDeviceIdle();
            needsReset = true;
            if (m_computeShader->GetShader()->Compile("Assets/Shaders/Compute/RT_compute.glsl", true))
                m_computeShader->GetMaterial()->OnShaderReloaded();
        }
        ImGui::Checkbox("Accumulate", &m_rtSettings.accumulate);
        if (ImGui::SliderInt("Rays Per Pixel", &m_rtSettings.raysPerPixel, 1, 20))
        {
            m_rayMaterial->SetConstant("pData", "RaysPerPixel", &m_rtSettings.raysPerPixel, sizeof(int));
            needsReset = true;
        }
        if (ImGui::SliderInt("Max Bounces", &m_rtSettings.maxBounces, 1, 20))
        {
            m_rayMaterial->SetConstant("pData", "MaxBounces", &m_rtSettings.maxBounces, sizeof(int));
            needsReset = true;
        }

        ImGui::Separator();
        if (ImGui::ColorEdit3("Sky Color Horizon", &m_rtSettings.skyColorHorizon[0]))
        {
            m_rayMaterial->SetConstant("pData", "SkyColorHorizon", &m_rtSettings.skyColorHorizon, sizeof(glm::vec3));
            needsReset = true;
        }
        if (ImGui::ColorEdit3("Sky Color Zenith", &m_rtSettings.skyColorZenith[0]))
        {
            m_rayMaterial->SetConstant("pData", "SkyColorZenith", &m_rtSettings.skyColorZenith, sizeof(glm::vec3));
            needsReset = true;
        }
        if (ImGui::ColorEdit3("Ground Color", &m_rtSettings.groundColor[0]))
        {
            m_rayMaterial->SetConstant("pData", "GroundColor", &m_rtSettings.groundColor, sizeof(glm::vec3));
            needsReset = true;
        }
        if (ImGui::ColorEdit3("Sun Color", &m_rtSettings.sunColor[0]))
        {
            m_rayMaterial->SetConstant("pData", "SunColor", &m_rtSettings.sunColor, sizeof(glm::vec3));
            needsReset = true;
        }
        if (ImGui::DragFloat3("Sun Direction", &m_rtSettings.sunDirection[0], 0.1f))
        {
            m_rayMaterial->SetConstant("pData", "SunDirection", &m_rtSettings.sunDirection, sizeof(glm::vec3));
            needsReset = true;
        }
        if (ImGui::SliderFloat("Sun Focus", &m_rtSettings.sunFocus, 0.1f, 1000.0f))
        {
            m_rayMaterial->SetConstant("pData", "SunFocus", &m_rtSettings.sunFocus, sizeof(float));
            needsReset = true;
        }
        if (ImGui::SliderFloat("Sun Intensity", &m_rtSettings.sunIntensity, 0.1f, 1000.0f))
        {
            m_rayMaterial->SetConstant("pData", "SunIntensity", &m_rtSettings.sunIntensity, sizeof(float));
            needsReset = true;
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

        ImGui::Begin("Scene");
        if (ImGui::Button("Add Cube"))
        {
            AddMesh(m_cubeMesh, "Cube");
        }   
        ImGui::SameLine();
        if (ImGui::Button("Add Cylinder"))
        {
            AddMesh(m_cylinderMesh, "Cylinder");
        }
        ImGui::SameLine();
        if (ImGui::Button("Add Sphere"))
        {
            auto entity = m_scene->CreateEntity("Sphere " + std::to_string(m_numTotalSpheres));
            entity.AddComponent<RayTracingSphereComponent>();

            m_numTotalSpheres += 1;
        }
        if (ImGui::Button("Add Teapot"))
        {
            AddMesh(m_teapotMesh, "Teapot");
        }
        ImGui::SameLine();
        if (ImGui::Button("Add Suzanne"))
        {
            AddMesh(m_suzanneMesh, "Suzanne");
        }
        m_scene->GetRegistry().view<RayTracingMeshComponent>().each([&](auto entityID, RayTracingMeshComponent& rtComp)
        {
            ImGui::PushID((void*)entityID);
            Entity entity = { entityID, m_scene.get() };
            if (ImGui::CollapsingHeader(entity.GetName().c_str()))
            {
                auto& transComp = entity.GetComponent<TransformComponent>();

                needsReset |= ImGui::DragFloat3("Position", &transComp.transform.position[0], 0.1f);
                needsReset |= ImGui::DragFloat3("Rotation", &transComp.transform.rotation[0], 0.1f);
                needsReset |= ImGui::DragFloat3("Scale", &transComp.transform.scale[0], 0.1f);
                ImGui::Spacing();

                needsReset |= ImGui::ColorEdit3("Albedo", &rtComp.material.albedo[0]);
                needsReset |= ImGui::SliderFloat("Smoothness", &rtComp.material.smoothness, 0.0f, 1.0f);
                needsReset |= ImGui::ColorEdit3("Specular Color", &rtComp.material.specularColor[0]);
                needsReset |= ImGui::SliderFloat("Specular Probability", &rtComp.material.specularProb, 0.0f, 1.0f);
                needsReset |= ImGui::ColorEdit3("Emission Color", &rtComp.material.emissionColor[0]);
                needsReset |= ImGui::SliderFloat("Emission Strength", &rtComp.material.emissionStrength, 0.0f, 1.0f);
                ImGui::Separator();
            }

            ImGui::PopID();
        });
        m_scene->GetRegistry().view<RayTracingSphereComponent>().each([&](auto entityID, RayTracingSphereComponent& rtComp)
        {
            ImGui::PushID((void*)entityID);
            Entity entity = { entityID, m_scene.get() };
            if (ImGui::CollapsingHeader(entity.GetName().c_str()))
            {
                auto& transComp = entity.GetComponent<TransformComponent>();

                needsReset |= ImGui::DragFloat3("Position", &transComp.transform.position[0], 0.1f);
                needsReset |= ImGui::DragFloat("Radius", &rtComp.radius, 0.1f);
                ImGui::Spacing();

                needsReset |= ImGui::ColorEdit3("Albedo", &rtComp.material.albedo[0]);
                needsReset |= ImGui::SliderFloat("Smoothness", &rtComp.material.smoothness, 0.0f, 1.0f);
                needsReset |= ImGui::ColorEdit3("Specular Color", &rtComp.material.specularColor[0]);
                needsReset |= ImGui::SliderFloat("Specular Probability", &rtComp.material.specularProb, 0.0f, 1.0f);
                needsReset |= ImGui::ColorEdit3("Emission Color", &rtComp.material.emissionColor[0]);
                needsReset |= ImGui::SliderFloat("Emission Strength", &rtComp.material.emissionStrength, 0.0f, 1.0f);
                ImGui::Separator();
            }
            ImGui::PopID();
        });
        ImGui::End();

        ImGui::Begin("Stats");
        ImGui::Text("FPS: %f", Stats::Get().fps);
        ImGui::Separator();
        ImGui::Text("Vertices: %d", m_numTotalVertices);
        ImGui::Text("Indices: %d", m_numTotalIndices);
        ImGui::Text("Meshes: %d", m_numTotalMeshes);
        ImGui::End();

        if (needsReset)
        {
            ResetPixelBuffer();
        }
    }
    void ComputeLayer::OnUpdate(float deltaTime)
    {
        Vec<RayTracingMaterial> materials(m_numTotalMeshes + m_numTotalSpheres);
        Vec<RayTracingSphereInfo> spheres(m_numTotalSpheres);
        Vec<glm::mat4> transforms(m_numTotalMeshes);

        m_scene->GetRegistry().view<RayTracingMeshComponent>().each([&](auto entityID, RayTracingMeshComponent& rtComp)
        {
            Entity entity = { entityID, m_scene.get() };
            auto& transfomMatrix = entity.GetComponent<TransformComponent>().GetTransformMatrix(); // TODO: Optimize

            materials[rtComp.index] = rtComp.material;
            transforms[rtComp.index] = transfomMatrix;
        });
        int i = 0;
        m_scene->GetRegistry().view<RayTracingSphereComponent>().each([&](auto entityID, RayTracingSphereComponent& rtComp)
        {
            Entity entity = { entityID, m_scene.get() };

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

        if (m_controlCamera && m_cursorDisabled)
            ControlCamera(deltaTime);
    }
    void ComputeLayer::OnCompute(float deltaTime)
    {
        RenderCommand::Dispatch(m_computeShader, m_numTotalVertices / 3, 1, 1);
    }
    void ComputeLayer::OnRender(float deltaTime)
    {
        ++m_frameIndex;

        m_rayMaterial->SetConstant("pData", "CameraPos", &m_cameraTransform.position[0], sizeof(glm::vec3));
        m_rayMaterial->SetConstant("pData", "Frames", &m_frameIndex, sizeof(uint32_t));
        m_rayMaterial->SetConstant("pData", "Accumulate", &m_rtSettings.accumulate, sizeof(uint32_t));

        Renderer::UpdateViewProjection(m_camera, m_cameraTransform);
        RenderCommand::DrawIndexed(m_fullscreenVA, m_rayMaterial.get());
    }

    void ComputeLayer::OnAPIHasInitialized(GraphicsAPI::Type api)
    {
        InitQuad();
        InitShader();
        InitMeshes();

        const uint32_t maxVerticies = 1000000;
        const uint32_t maxIndices = 1000000;
        const uint32_t maxMeshes = 100;
        const uint32_t maxSpheres = 100;
        Renderer::CreateStorageBuffer("bVertices", maxVerticies * sizeof(float), 2);
        Renderer::CreateStorageBuffer("bTransVertices", maxVerticies * sizeof(float), 3);
        Renderer::CreateStorageBuffer("bIndices", maxIndices * sizeof(uint32_t), 4);
        Renderer::CreateStorageBuffer("bSpheres", 4 * sizeof(uint32_t) + maxSpheres * sizeof(RayTracingSphereInfo), 5);
        Renderer::CreateStorageBuffer("bMeshes", 4 * sizeof(uint32_t) + maxMeshes * sizeof(RayTracingMeshInfo), 6); 
        Renderer::CreateStorageBuffer("bMaterials", (maxMeshes + maxSpheres) * sizeof(RayTracingMaterial), 7);
        Renderer::CreateStorageBuffer("bTransforms", maxMeshes * sizeof(glm::mat4), 8);

        InitMaterial();
        InitComputeShader();
    }
    void ComputeLayer::OnShutdownAPI(bool quit)
    {
        m_fullscreenVA->Dispose();
        m_rayShader->Dispose();
        m_rayMaterial->Dispose();
        m_computeShader->Dispose();
        m_cubeMesh->Dispose();
        m_teapotMesh->Dispose();
        m_suzanneMesh->Dispose();
        m_cylinderMesh->Dispose();
    }

    void ComputeLayer::InitQuad()
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
    void ComputeLayer::InitShader()
    {
        m_rayShader = Shader::Create("Assets/Shaders/Compute/RT_mesh.glsl");
    }
    void ComputeLayer::InitMaterial()
    {
        glm::vec2 screenSize = { Application::Get().GetInfo().windowWidth, Application::Get().GetInfo().windowHeight };

        size_t bufferSize = screenSize.x * screenSize.y * 4 * sizeof(float);
        Renderer::CreateStorageBuffer("bPixels", bufferSize, 1);

        m_rayMaterial = Material::Create(m_rayShader);

        m_rayMaterial->SetConstant("pData", "Resolution", &screenSize, sizeof(glm::vec2));
        m_rayMaterial->SetConstant("pData", "RaysPerPixel", &m_rtSettings.raysPerPixel, sizeof(int));
        m_rayMaterial->SetConstant("pData", "MaxBounces", &m_rtSettings.maxBounces, sizeof(int));

        m_rayMaterial->SetConstant("pData", "SkyColorHorizon", &m_rtSettings.skyColorHorizon, sizeof(glm::vec3));
        m_rayMaterial->SetConstant("pData", "SkyColorZenith", &m_rtSettings.skyColorZenith, sizeof(glm::vec3));
        m_rayMaterial->SetConstant("pData", "GroundColor", &m_rtSettings.groundColor, sizeof(glm::vec3));
        m_rayMaterial->SetConstant("pData", "SunColor", &m_rtSettings.sunColor, sizeof(glm::vec3));
        m_rayMaterial->SetConstant("pData", "SunDirection", &m_rtSettings.sunDirection, sizeof(glm::vec3));
        m_rayMaterial->SetConstant("pData", "SunFocus", &m_rtSettings.sunFocus, sizeof(float));
        m_rayMaterial->SetConstant("pData", "SunIntensity", &m_rtSettings.sunIntensity, sizeof(float));
    }
    void ComputeLayer::InitMeshes()
    {
        m_cubeMesh = Mesh::Create("Assets/Meshes/cube.obj");
        m_teapotMesh = Mesh::Create("Assets/Meshes/teapot.obj");
        m_suzanneMesh = Mesh::Create("Assets/Meshes/suzanne.obj");
        m_cylinderMesh = Mesh::Create("Assets/Meshes/cylinder.obj");
    }
    void ComputeLayer::InitComputeShader()
    {
        // Must be called after the storage buffers are created
        m_computeShader = ComputeShader::Create("Assets/Shaders/Compute/RT_compute.glsl"); 
    }
    void ComputeLayer::ResetPixelBuffer()
    {
        m_frameIndex = 0;
    }

    void ComputeLayer::ControlCamera(float deltaTime)
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
    void ComputeLayer::AddMesh(const Ref<Mesh> mesh, const String& name)
    {
        Renderer::WaitDeviceIdle();

        auto entity = m_scene->CreateEntity(name + " " + std::to_string(m_numTotalMeshes));
        entity.AddComponent<MeshComponent>(mesh.get(), nullptr);
        auto& rtMesh = entity.AddComponent<RayTracingMeshComponent>();
        rtMesh.index = m_numTotalMeshes;

        Vec<float> vertices = mesh->GetVertices();
        auto buffer = Renderer::GetStorageBuffer("bVertices");
        buffer->SetData(vertices.data(), vertices.size() * sizeof(float), m_numTotalVertices * sizeof(float));

        Vec<uint32_t> indices = mesh->GetIndices();
        buffer = Renderer::GetStorageBuffer("bIndices");
        buffer->SetData(indices.data(), indices.size() * sizeof(uint32_t), m_numTotalIndices * sizeof(uint32_t));

        Bounds bounds = mesh->GetBounds();

        RayTracingMeshInfo meshInfo{};
        meshInfo.vertexOffset = m_numTotalVertices;
        meshInfo.vertexCount = vertices.size();
        meshInfo.indexOffset = m_numTotalIndices;
        meshInfo.indexCount = indices.size();
        meshInfo.boundMin = bounds.min;
        meshInfo.boundMax = bounds.max;
        meshInfo.materialIndex = m_numTotalMeshes;

        m_numTotalMeshes += 1;
        buffer = Renderer::GetStorageBuffer("bMeshes");
        buffer->SetData(&m_numTotalMeshes, sizeof(uint32_t));
        buffer->SetData(&meshInfo, sizeof(RayTracingMeshInfo), 4 * sizeof(uint32_t) + (m_numTotalMeshes - 1u) * sizeof(RayTracingMeshInfo));

        m_numTotalVertices += vertices.size();
        m_numTotalIndices += indices.size();
    }
}