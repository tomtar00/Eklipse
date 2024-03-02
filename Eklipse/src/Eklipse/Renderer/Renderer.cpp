#include "precompiled.h"
#include "Renderer.h"
#include "RenderCommand.h"
#include "Material.h"

#include <Eklipse/Utils/Stats.h>
#include <Eklipse/Scene/Components.h>
#include <Eklipse/Core/Application.h>
#include <Eklipse/Platform/Vulkan/VulkanAPI.h>
#include <Eklipse/Platform/OpenGL/OpenGLAPI.h>

// Force the use of the dedicated GPU on laptops
#ifdef __cplusplus
extern "C" {
#endif

    __declspec(dllexport) uint32_t NvOptimusEnablement = 1;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;

#ifdef __cplusplus
}
#endif

namespace Eklipse
{
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

    std::unordered_map<String, Ref<UniformBuffer>, std::hash<String>>	Renderer::s_uniformBufferCache;
    std::unordered_map<String, Ref<StorageBuffer>, std::hash<String>>	Renderer::s_storageBufferCache;

    RendererSettings Renderer::s_settings;
    Unique<RendererContext> Renderer::s_rendererContext = nullptr;
    Ref<UniformBuffer> Renderer::s_cameraUniformBuffer = nullptr;
    Ref<Framebuffer> Renderer::s_defaultFramebuffer = nullptr;

    bool Renderer::Init()
    {
        EK_CORE_PROFILE();
        RenderCommand::API.reset();
        RenderCommand::API = GraphicsAPI::Create();
        SetPipelineTopologyMode(Pipeline::TopologyMode::Triangle);
        SetPipelineType(Pipeline::Type::Resterization);
        return RenderCommand::API->Init();
    }
    void Renderer::InitParameters()
    {
        EK_CORE_PROFILE();
        s_cameraUniformBuffer = Renderer::CreateUniformBuffer("uCamera", sizeof(glm::mat4), 0);

        FramebufferInfo framebufferInfo{};
        framebufferInfo.isDefaultFramebuffer = true;
        framebufferInfo.width = Application::Get().GetInfo().windowWidth;
        framebufferInfo.height = Application::Get().GetInfo().windowHeight;
        framebufferInfo.numSamples = 1;
        framebufferInfo.colorAttachmentInfos = { { ImageFormat::RGBA32F } };
        framebufferInfo.depthAttachmentInfo = { ImageFormat::D24S8 };

        s_defaultFramebuffer = Framebuffer::Create(framebufferInfo);
    }
    void Renderer::WaitDeviceIdle()
    {
        RenderCommand::API->WaitDeviceIdle();
    }
    void Renderer::Shutdown()
    {
        EK_CORE_PROFILE();

        for (auto&& [name, uniformBuffer] : s_uniformBufferCache)
        {
            uniformBuffer->Dispose();
        }
        s_uniformBufferCache.clear();

        for (auto&& [name, storageBuffer] : s_storageBufferCache)
        {
            storageBuffer->Dispose();
        }
        s_storageBufferCache.clear();

        g_defaultFramebuffer->Dispose();
        for (auto& framebuffer : g_offScreenFramebuffers)
        {
            framebuffer->Dispose();
        }
        g_offScreenFramebuffers.clear();

        Pipeline::DisposeAll();

        s_rendererContext->Shutdown();
        s_rendererContext = nullptr;

        RenderCommand::API->Shutdown();
    }

    // Render stages
    void Renderer::BeginFrame()
    {
        EK_PROFILE();

        Stats::Get().Reset();
        RenderCommand::API->BeginFrame();
    }
    void Renderer::UpdateViewProjection(Camera& camera, Transform& cameraTransform)
    {
        camera.UpdateViewProjectionMatrix(cameraTransform, g_currentFramebuffer->GetAspectRatio());
        s_cameraUniformBuffer->SetData(&camera.GetViewProjectionMatrix(), sizeof(glm::mat4));
    }
    void Renderer::RenderScene(Ref<Scene> scene, Camera& camera, Transform& cameraTransform)
    {
        EK_PROFILE();
        UpdateViewProjection(camera, cameraTransform);
        s_rendererContext->RenderScene(scene, camera, cameraTransform);
    }
    void Renderer::RenderScene(Ref<Scene> scene)
    {
        EK_PROFILE();
        auto camera = scene->GetMainCamera();
        auto cameraTransform = scene->GetMainCameraTransform();

        if (!camera)
        {
            EK_CORE_ERROR("No main camera present on the scene!");
            return;
        }
        else if (!cameraTransform)
        {
            EK_CORE_ERROR("Main camera transform is null!");
            return;
        }
        else
            RenderScene(scene, *camera, *cameraTransform);
    }
    void Renderer::Submit()
    {
        EK_PROFILE();

        RenderCommand::API->Submit();
    }

    // Render calls
    void Renderer::BeginRenderPass(Framebuffer* framebuffer)
    {
        EK_PROFILE();

        framebuffer->Bind();
    }
    void Renderer::EndRenderPass(Framebuffer* framebuffer)
    {
        EK_PROFILE();

        framebuffer->Unbind();
    }

    // Events
    void Renderer::OnWindowResize(uint32_t width, uint32_t height)
    {
        EK_CORE_PROFILE();
        s_defaultFramebuffer->Resize(width, height);
    }
    void Renderer::OnMultiSamplingChanged(uint32_t numSamples)
    {
        EK_CORE_PROFILE();
        if (s_settings.GetMsaaSamples() == numSamples) return;
        s_settings.MsaaSamplesIndex = numSamples >> 1;

        // TODO: Works with Vulkan, but not with OpenGL
        // ImGui doesn't work with multisampled GLTexture2Ds as ImGui::Image() input

        /*
        auto& fbInfo = g_sceneFramebuffer->GetInfo();
        fbInfo.numSamples = numSamples;
        g_sceneFramebuffer->Resize(fbInfo.width, fbInfo.height);
        */
    }
    void Renderer::OnVsyncChanged(bool enabled)
    {
        EK_CORE_PROFILE();
        if (s_settings.Vsync) return;
        s_settings.Vsync = enabled;

#ifdef EK_PLATFORM_WINDOWS
        glfwSwapInterval(enabled);
#endif
    }

    // State changing
    GraphicsAPI::Type Renderer::GetGraphicsAPIType()
    {
        return s_settings.GraphicsAPIType;
    }
    void Renderer::SetGraphicsAPIType(GraphicsAPI::Type apiType)
    {
        s_settings.GraphicsAPIType = apiType;
    }
    void Renderer::SetPipelineTopologyMode(Pipeline::TopologyMode mode)
    {
        EK_CORE_PROFILE();
        RenderCommand::API->SetPipelineTopologyMode(mode);
        s_settings.PipelineTopologyMode = mode;
    }
    void Renderer::SetPipelineType(Pipeline::Type type)
    {
        EK_CORE_PROFILE();
        if (s_settings.PipelineType == type && s_rendererContext)
            return;

        RenderCommand::API->SetPipelineType(type);
        s_settings.PipelineType = type;

        // WaitDeviceIdle();

        if (s_rendererContext)
            s_rendererContext->Shutdown();

        if (type == Pipeline::Type::Resterization)
            s_rendererContext = CreateUnique<RasterizationContext>();
        else if (type == Pipeline::Type::RayTracing)
            s_rendererContext = CreateUnique<RayTracingContext>();

        s_rendererContext->Init();
    }

    // Uniform buffers
    Ref<UniformBuffer> Renderer::CreateUniformBuffer(const String& uniformBufferName, const size_t size, const uint32_t binding)
    {
        EK_CORE_PROFILE();
        if (s_uniformBufferCache.find(uniformBufferName) != s_uniformBufferCache.end())
        {
            return s_uniformBufferCache[uniformBufferName];
        }

        Ref<UniformBuffer> uniformBuffer = UniformBuffer::Create(size, binding);
        s_uniformBufferCache[uniformBufferName] = uniformBuffer;
        EK_CORE_DBG("Created uniform buffer '{0}' with size {1} and binding {2}", uniformBufferName, size, binding);
        return uniformBuffer;
    }
    Ref<UniformBuffer> Renderer::GetUniformBuffer(const String& uniformBufferName)
    {
        EK_CORE_PROFILE();
        if (s_uniformBufferCache.find(uniformBufferName) != s_uniformBufferCache.end())
        {
            return s_uniformBufferCache[uniformBufferName];
        }

        EK_ASSERT(false, "Uniform buffer '{0}' not found", uniformBufferName);
        return nullptr;
    }

    // Storage buffers
    Ref<StorageBuffer> Renderer::CreateStorageBuffer(const String& storageBufferName, const size_t size, const uint32_t binding)
    {
        EK_CORE_PROFILE();
        if (s_storageBufferCache.find(storageBufferName) != s_storageBufferCache.end())
        {
            s_storageBufferCache.at(storageBufferName)->Dispose();
            s_storageBufferCache.at(storageBufferName).reset();
        }

        Ref<StorageBuffer> storageBuffer = StorageBuffer::Create(size, binding);
        s_storageBufferCache[storageBufferName] = storageBuffer;
        EK_CORE_DBG("Created storage buffer '{0}' with size {1} and binding {2}", storageBufferName, size, binding);
        return storageBuffer;
    }
    Ref<StorageBuffer> Renderer::GetStorageBuffer(const String& storageBufferName)
    {
        EK_CORE_PROFILE();
        if (s_storageBufferCache.find(storageBufferName) != s_storageBufferCache.end())
        {
            return s_storageBufferCache[storageBufferName];
        }

        EK_ASSERT(false, "Storage buffer '{0}' not found", storageBufferName);
        return nullptr;
    }

    // Settings
    const RendererSettings& Renderer::GetSettings()
    {
        return s_settings;
    }
    void Renderer::SerializeRendererSettings(YAML::Emitter& out)
    {
        EK_CORE_PROFILE();
        out << YAML::Key << "RendererSettings" << YAML::Value;
        {
            out << YAML::BeginMap;
            out << YAML::Key << "Vsync" << YAML::Value << s_settings.Vsync;
            out << YAML::Key << "MsaaSamplesIndex" << YAML::Value << s_settings.MsaaSamplesIndex;
            out << YAML::EndMap;
        }
    }
    void Renderer::DeserializeRendererSettings(const YAML::Node& data)
    {
        EK_CORE_PROFILE();
        s_settings.Vsync = TryDeserailize<bool>(data, "Vsync", false);
        OnVsyncChanged(s_settings.Vsync);

        s_settings.MsaaSamplesIndex = TryDeserailize<int>(data, "MsaaSamplesIndex", 0);
        OnMultiSamplingChanged(s_settings.MsaaSamplesIndex);
    }
    
}