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
    const String APITypeToString(ApiType apiType)
    {
        switch (apiType)
        {
        case ApiType::Vulkan: return "Vulkan";
        case ApiType::OpenGL: return "OpenGL";
        }
        return "Unknown";
    }

    RendererSettings Renderer::s_settings;
    ApiType	Renderer::s_apiType = ApiType::Vulkan;
    std::unordered_map<String, Ref<UniformBuffer>, std::hash<String>>	Renderer::s_uniformBufferCache;
    static Ref<UniformBuffer> s_cameraUniformBuffer;

    bool Renderer::Init()
    {
        EK_CORE_PROFILE();
        RenderCommand::API.reset();
        RenderCommand::API = GraphicsAPI::Create();
        return RenderCommand::API->Init();
    }
    void Renderer::InitParameters()
    {
        s_cameraUniformBuffer = Renderer::CreateUniformBuffer("uCamera", sizeof(glm::mat4), 0);
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
        RenderCommand::API->Shutdown();
    }

    // Render stages
    void Renderer::BeginFrame()
    {
        EK_PROFILE();

        Stats::Get().Reset();
        RenderCommand::API->BeginFrame();
    }
    void Renderer::BeginDefaultRenderPass()
    {
        EK_PROFILE();

        RenderCommand::API->BeginDefaultRenderPass();
    }
    void Renderer::EndDefaultRenderPass()
    {
        EK_PROFILE();

        RenderCommand::API->EndDefaultRenderPass();
    }
    void Renderer::RenderScene(Ref<Scene> scene, Camera& camera, Transform& cameraTransform)
    {
        EK_PROFILE();

        camera.UpdateViewProjectionMatrix(cameraTransform, g_currentFramebuffer->GetAspectRatio());
        s_cameraUniformBuffer->SetData(&camera.GetViewProjectionMatrix(), sizeof(glm::mat4));

        // Geometry
        auto view = scene->GetRegistry().view<TransformComponent, MeshComponent>();
        for (auto& entity : view)
        {
            auto [transformComponent, meshComponent] = view.get<TransformComponent, MeshComponent>(entity);

#ifdef EK_DEBUG
            if (meshComponent.mesh == nullptr || meshComponent.material == nullptr || !meshComponent.material->IsValid()) continue;
#endif

            glm::mat4& modelMatrix = transformComponent.GetTransformMatrix();
            meshComponent.material->SetConstant("uVertConst", "Model", &modelMatrix[0][0], sizeof(glm::mat4));
            RenderCommand::DrawIndexed(meshComponent.mesh->GetVertexArray(), meshComponent.material);
        }

        // ...
    }
    void Renderer::RenderScene(Ref<Scene> scene)
    {
        EK_PROFILE();
        auto camera = scene->GetMainCamera();
        auto cameraTransform = scene->GetMainCameraTransform();

#ifdef EK_DEBUG
        if (!camera)
        {
            EK_CORE_ERROR("No main camera present on the scene!");
            return;
        }
        else if (!cameraTransform)
        {
            EK_CORE_ERROR("Main camera transform is null!");
        }
        else
#endif
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
        RenderCommand::API->OnWindowResize(width, height);
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

    // Getters / Setters
    ApiType Renderer::GetAPI()
    {
        return s_apiType;
    }
    void Renderer::SetStartupAPI(ApiType apiType)
    {
        s_apiType = apiType;
    }
    void Renderer::SetAPI(ApiType apiType)
    {
        s_apiType = apiType;
    }
    RendererSettings& Renderer::GetSettings()
    {
        return s_settings;
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

    // Settings
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