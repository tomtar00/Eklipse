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
    void Pipeline::DeleteUnsused()
    {
        for (auto it = s_pipelines.begin(); it != s_pipelines.end(); ++it)
        {
            float timeSinceLastAccessMs = Timer::DurationMs(it->second->m_lastAccessTime, Timer::Now());
            if (it->second.use_count() == 1 && timeSinceLastAccessMs > EK_PIPELINE_LIFETIME)
            {
                it->second->Dispose();
                it = s_pipelines.erase(it);
            }
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
