#include "precompiled.h"
#include "Pipeline.h"
#include "Renderer.h"

#include <Eklipse/Utils/Hash.h>

#include <Eklipse/Platform/Vulkan/VKPipeline.h>
#include <Eklipse/Platform/OpenGL/GLPipeline.h>

namespace Eklipse
{
    std::map<size_t, Ref<Pipeline>> Pipeline::s_pipelines;

    Pipeline::Pipeline(const Config& config) : m_config(config) 
    {
        EK_CORE_PROFILE();
        EK_ASSERT(config.shader, "Shader is null");

        m_lastAccessTime = Timer::Now();
    }

    Ref<Pipeline> Pipeline::Get(const Config& config)
    {
        EK_CORE_PROFILE();

        // hash the config
        size_t hash = 0;
        HashCombine(hash, config.type);
        HashCombine(hash, config.topologyMode);
        HashCombine(hash, config.shader);
        HashCombine(hash, config.framebuffer);
        HashCombine(hash, config.depthTest);

        // check if the pipeline already exists
        auto it = s_pipelines.find(hash);
        if (it != s_pipelines.end())
        {
            it->second->m_lastAccessTime = Timer::Now();
            return it->second;
        }

        // create the pipeline
        Ref<Pipeline> pipeline = Create(config);
        s_pipelines[hash] = pipeline;
        return pipeline;
    }
    Vec<Ref<Pipeline>> Pipeline::GetPipelinesByShader(const AssetHandle shaderHandle)
    {
        Vec<Ref<Pipeline>> pipelines;
        for (auto& [hash, pipeline] : s_pipelines)
        {
            if (pipeline->m_config.shader->Handle == shaderHandle)
            {
                pipelines.push_back(pipeline);
            }
        }
        return pipelines;
    }
    String Pipeline::TypeToString(const Type& type)
    {
        if (type == Type::Rasterization) return "Rasterization";
        if (type == Type::Compute) return "Compute";
        if (type == Type::RayTracing) return "Ray Tracing";
        if (type == Type::RayTracingKHR) return "Ray Tracing (KHR)";

        EK_ASSERT(false, "Unknown pipeline type");
        return "Unknown";
    }
    String Pipeline::TopologyModeToString(const TopologyMode& topologyMode)
    {
        if (topologyMode == TopologyMode::Triangle) return "Triangle";
        if (topologyMode == TopologyMode::Line) return "Line";

        EK_ASSERT(false, "Unknown topology mode");
        return "Unknown";
    }
    Pipeline::Type Pipeline::StringToType(const String& type)
    {
        if (type == "Rasterization") return Type::Rasterization;
        if (type == "Compute") return Type::Compute;
        if (type == "Ray Tracing") return Type::RayTracing;
        if (type == "Ray Tracing (KHR)") return Type::RayTracingKHR;

        EK_ASSERT(false, "Unknown pipeline type: {0}", type);
        return Type::Rasterization;
    }
    Pipeline::TopologyMode Pipeline::StringToTopologyMode(const String& topologyMode)
    {
        if (topologyMode == "Triangle") return TopologyMode::Triangle;
        if (topologyMode == "Line") return TopologyMode::Line;

        EK_ASSERT(false, "Unknown topology mode: {0}", topologyMode);
        return TopologyMode::Triangle;
    }
    void Pipeline::DeleteUnsused()
    {
        EK_CORE_PROFILE();
        Vec<size_t> toDelete;

        for (auto it = s_pipelines.begin(); it != s_pipelines.end(); ++it)
        {
            float timeSinceLastAccessMs = Timer::DurationMs(it->second->m_lastAccessTime, Timer::Now());
            if (it->second.use_count() == 1 && timeSinceLastAccessMs > EK_PIPELINE_LIFETIME)
            {
                EK_CORE_TRACE("Pipeline {} will be deleted", it->first);
                toDelete.push_back(it->first);
            }
        }

        if (!toDelete.empty()) 
        {
            Renderer::WaitDeviceIdle();
        }
        for (auto& hash : toDelete)
        {
            EK_CORE_TRACE("Disposing unsused pipeline ({})", hash);
            s_pipelines.at(hash)->Dispose();
            s_pipelines.erase(hash);
            EK_CORE_DBG("Unsused pipeline disposed");
        }
    }
    void Pipeline::DisposeAll()
    {
        for (auto& [hash, pipeline] : s_pipelines)
        {
            pipeline->Dispose();
        }
        s_pipelines.clear();

        auto graphicsAPI = Renderer::GetGraphicsAPIType();
        if (graphicsAPI == GraphicsAPI::Type::Vulkan)
        {
            Vulkan::VKPipeline::DisposeCache();
        }
    }
    
    Ref<Pipeline> Pipeline::Create(const Config& config)
    {
        EK_CORE_PROFILE();
        switch (Renderer::GetGraphicsAPIType())
        {
            case GraphicsAPI::Type::Vulkan: return CreateRef<Vulkan::VKPipeline>(config);
            case GraphicsAPI::Type::OpenGL: return CreateRef<OpenGL::GLPipeline>(config);
        }
        EK_ASSERT(false, "Shader creation not implemented for current graphics API");
        return nullptr;
    }
}
