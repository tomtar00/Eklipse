#include "precompiled.h"
#include "Renderer.h"
#include "RenderCommand.h"
#include "Material.h"

#include <Eklipse/Utils/Stats.h>
#include <Eklipse/Scene/Components.h>
#include <Eklipse/Core/Application.h>
#include <Eklipse/Platform/Vulkan/VulkanAPI.h>
#include <Eklipse/Platform/OpenGL/OpenGLAPI.h>
#include <Eklipse/Renderer/RendererContext.h>

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
    std::unordered_map<String, Ref<UniformBuffer>, std::hash<String>>	Renderer::s_uniformBufferCache;
    std::unordered_map<String, Ref<StorageBuffer>, std::hash<String>>	Renderer::s_storageBufferCache;

    RendererSettings Renderer::s_settings;

    GraphicsAPI::Type Renderer::s_targetAPIType = Renderer::s_settings.GraphicsAPIType;
    Pipeline::Type Renderer::s_targetPipelineType;
    bool Renderer::s_pipelineTypeChangeRequeted;

    Ref<RendererContext> Renderer::s_rendererContext = nullptr;
    Ref<UniformBuffer> Renderer::s_cameraUniformBuffer = nullptr;
    Ref<Framebuffer> Renderer::s_defaultFramebuffer = nullptr;

    bool Renderer::Init(GraphicsAPI::Type apiType)
    {
        EK_CORE_PROFILE();
        RenderCommand::API.reset();
        RenderCommand::API = GraphicsAPI::Create(apiType);
        s_settings.GraphicsAPIType = apiType;
        if (RenderCommand::API->Init())
        {
            AssetManager::ReloadAssets();

            if (SceneManager::GetActiveScene())
                SceneManager::GetActiveScene()->ApplyAllComponents();

            SetPipelineTopologyMode(s_settings.PipelineTopologyMode);
            SetPipelineType(s_settings.PipelineType);
            return true;
        }
        return false;
    }
    void Renderer::InitSSBOs()
    {
        EK_CORE_PROFILE();
        s_rendererContext->InitSSBOs();
    }
    void Renderer::OnAPIHasInitialized()
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
        s_rendererContext.reset();
        s_rendererContext = nullptr;

        RenderCommand::API->Shutdown();
    }
    void Renderer::WaitDeviceIdle()
    {
        EK_PROFILE();
        RenderCommand::API->WaitDeviceIdle();
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
    void Renderer::OnUpdate(float deltaTime)
    {
        EK_PROFILE();
        s_rendererContext->OnUpdate(deltaTime);
    }
    void Renderer::BeginComputePass()
    {
        EK_PROFILE();
        RenderCommand::API->BeginComputePass();
    }
    void Renderer::OnCompute(float deltaTime)
    {
        EK_PROFILE();
        s_rendererContext->OnCompute(deltaTime);
    }
    void Renderer::EndComputePass()
    {
        EK_PROFILE();
        RenderCommand::API->EndComputePass();
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

        if (s_pipelineTypeChangeRequeted)
        {
            SetPipelineType(s_targetPipelineType);
            s_pipelineTypeChangeRequeted = false;
        }
        Pipeline::DeleteUnsused();
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
        s_rendererContext->OnWindowResize(width, height);
    }
    void Renderer::OnMultiSamplingChanged(Framebuffer* framebuffer, uint32_t numSamples)
    {
        EK_CORE_PROFILE();
        if (s_settings.GetMsaaSamples() == numSamples) return;
        s_settings.MsaaSamplesIndex = numSamples >> 1;

        // TODO: Not working
        // ImGui doesn't work with multisampled GLTexture2Ds as ImGui::Image() input

        /*
        auto& fbInfo = framebuffer->GetInfo();
        fbInfo.numSamples = numSamples;
        framebuffer->Resize(fbInfo.width, fbInfo.height);
        */
    }
    void Renderer::OnPresentModeChanged(PresentMode mode)
    {
        EK_CORE_PROFILE();
        if (s_settings.presentMode == mode) return;
        s_settings.presentMode = mode;

        if (s_settings.GraphicsAPIType == GraphicsAPI::Type::OpenGL)
        {
            glfwSwapInterval(mode != PresentMode::IMMEDIATE);
        }
        else if (s_settings.GraphicsAPIType == GraphicsAPI::Type::Vulkan)
        {
            g_defaultFramebuffer->Resize(g_defaultFramebuffer->GetInfo().width, g_defaultFramebuffer->GetInfo().height);
        }
    }
    void Renderer::OnMeshAdded(Entity entity)
    {
        s_rendererContext->OnMeshAdded(entity);
    }
    void Renderer::OnSphereAdded(Entity entity)
    {
        s_rendererContext->OnSphereAdded(entity);
    }
    void Renderer::OnSceneChanged()
    {
        EK_CORE_PROFILE();
        if (s_rendererContext)
        {
            s_rendererContext->OnSceneChanged();
        }
    }

    // State changing
    GraphicsAPI::Type Renderer::GetGraphicsAPIType()
    {
        return s_settings.GraphicsAPIType;
    }
    GraphicsAPI::Type Renderer::GetTargetGraphicsAPIType()
    {
        return s_targetAPIType;
    }
    Pipeline::Type Renderer::GetPipelineType()
    {
        return s_settings.PipelineType;
    }
    Ref<RendererContext> Renderer::GetRendererContext()
    {
        return s_rendererContext;
    }
    void Renderer::SetTargetGraphicsAPIType(GraphicsAPI::Type apiType)
    {
        s_targetAPIType = apiType;
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
        EK_CORE_TRACE(" === Setting pipeline type to {0}", Pipeline::TypeToString(type));

        if (s_settings.PipelineType == type && s_rendererContext)
        {
            EK_CORE_WARN("Pipeline type already set to {0}", Pipeline::TypeToString(type));
            return;
        }

        WaitDeviceIdle();

        // For some reason, the storage buffers are not being disposed when the pipeline type changes
        for (auto&& [name, storageBuffer] : s_storageBufferCache)
        {
            storageBuffer->Dispose();
        }
        s_storageBufferCache.clear();

        RenderCommand::API->SetPipelineType(type);
        s_settings.PipelineType = type;

        if (s_rendererContext)
            s_rendererContext->Shutdown();

        s_rendererContext.reset();
        s_rendererContext = nullptr;

        if (type == Pipeline::Type::Resterization)
            s_rendererContext = CreateRef<RasterizationContext>();
        else if (type == Pipeline::Type::RayTracing)
            s_rendererContext = CreateRef<RayTracingContext>();

        s_rendererContext->Init();

        EK_CORE_DBG(" === Pipeline type set to {0}", Pipeline::TypeToString(type));
    }
    void Renderer::RequestPipelineTypeChange(Pipeline::Type type)
    {
        s_targetPipelineType = type;
        s_pipelineTypeChangeRequeted = true;
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
    RendererSettings& Renderer::GetSettings()
    {
        return s_settings;
    }
    void Renderer::SerializeRendererSettings(YAML::Emitter& out)
    {
        EK_CORE_PROFILE();
        out << YAML::Key << "RendererSettings" << YAML::Value;
        {
            out << YAML::BeginMap;
            out << YAML::Key << "PresentMode" << YAML::Value << PresentModeToString(s_settings.presentMode);
            out << YAML::Key << "MsaaSamplesIndex" << YAML::Value << s_settings.MsaaSamplesIndex;
            out << YAML::Key << "PipelineType" << YAML::Value << Pipeline::TypeToString(s_settings.PipelineType);
            out << YAML::Key << "PipelineTopologyMode" << YAML::Value << Pipeline::TopologyModeToString(s_settings.PipelineTopologyMode);
            out << YAML::Key << "GraphicsAPIType" << YAML::Value << GraphicsAPI::TypeToString(s_settings.GraphicsAPIType);
            out << YAML::Key << "SkyColorHorizon" << YAML::Value << s_settings.skyColorHorizon;
            out << YAML::Key << "SkyColorZenith" << YAML::Value << s_settings.skyColorZenith;
            out << YAML::Key << "GroundColor" << YAML::Value << s_settings.groundColor;
            out << YAML::Key << "SunColor" << YAML::Value << s_settings.sunColor;
            out << YAML::Key << "SunDirection" << YAML::Value << s_settings.sunDirection;
            out << YAML::Key << "SunFocus" << YAML::Value << s_settings.sunFocus;
            out << YAML::Key << "SunIntensity" << YAML::Value << s_settings.sunIntensity;
            out << YAML::Key << "Accumulate" << YAML::Value << s_settings.accumulate;
            out << YAML::Key << "RaysPerPixel" << YAML::Value << s_settings.raysPerPixel;
            out << YAML::Key << "MaxBounces" << YAML::Value << s_settings.maxBounces;
            out << YAML::EndMap;
        }
    }
    void Renderer::DeserializeRendererSettings(const YAML::Node& data)
    {
        EK_CORE_PROFILE();

        s_settings.presentMode = PresentModeFromString(TryDeserailize<String>(data, "PresentMode", "Immediate"));
        OnPresentModeChanged(s_settings.presentMode);

        s_settings.MsaaSamplesIndex = TryDeserailize<int>(data, "MsaaSamplesIndex", 0);
        //OnMultiSamplingChanged(g_defaultFramebuffer, s_settings.MsaaSamplesIndex); // TODO: not default framebuffer

        s_settings.skyColorHorizon = TryDeserailize<glm::vec3>(data, "SkyColorHorizon", { 1.0f, 1.0f, 1.0f });
        s_settings.skyColorZenith = TryDeserailize<glm::vec3>(data, "SkyColorZenith", { 0.07f, 0.36f, 0.72f });
        s_settings.groundColor = TryDeserailize<glm::vec3>(data, "GroundColor", { 0.35f, 0.3f, 0.35f });
        s_settings.sunColor = TryDeserailize<glm::vec3>(data, "SunColor", { 1.0f, 1.0f, 0.8f });
        s_settings.sunDirection = TryDeserailize<glm::vec3>(data, "SunDirection", { 0.0f, 0.3f, -1.0f });
        s_settings.sunFocus = TryDeserailize<float>(data, "SunFocus", 500.0f);
        s_settings.sunIntensity = TryDeserailize<float>(data, "SunIntensity", 200.0f);

        s_settings.accumulate = TryDeserailize<bool>(data, "Accumulate", false);
        s_settings.raysPerPixel = TryDeserailize<int>(data, "RaysPerPixel", 1);
        s_settings.maxBounces = TryDeserailize<int>(data, "MaxBounces", 3);

        Application::Get().SetGraphicsAPIType(s_settings.GraphicsAPIType);
        RequestPipelineTypeChange(Pipeline::StringToType(TryDeserailize<String>(data, "PipelineType", "Rasterization")));
        SetPipelineTopologyMode(Pipeline::StringToTopologyMode(TryDeserailize<String>(data, "PipelineTopologyMode", "Triangle")));
    }
}