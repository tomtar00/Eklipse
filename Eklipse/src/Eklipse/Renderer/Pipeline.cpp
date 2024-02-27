#include "precompiled.h"
#include "Pipeline.h"
#include "Renderer.h"

#include <Eklipse/Utils/Hash.h>

#include <Eklipse/Platform/Vulkan/VKPipeline.h>
#include <Eklipse/Platform/OpenGL/GLPipeline.h>

namespace Eklipse
{
    std::map<size_t, Ref<Pipeline>> Pipeline::s_pipelines;

    Pipeline::Pipeline(const Config& config) : m_config(config) {}

    Ref<Pipeline> Pipeline::Get(const Config& config)
    {
        EK_CORE_PROFILE();

        // hash the config
        size_t hash = 0;
        HashCombine(hash, config.type);
        HashCombine(hash, config.mode);
        HashCombine(hash, config.shader);
        HashCombine(hash, config.framebuffer);

        // check if the pipeline already exists
        auto it = s_pipelines.find(hash);
        if (it != s_pipelines.end())
        {
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
    
    Ref<Pipeline> Pipeline::Create(const Config& config)
    {
        EK_CORE_PROFILE();
        switch (Renderer::GetAPI())
        {
            case ApiType::Vulkan: return CreateRef<Vulkan::VKPipeline>(config);
            case ApiType::OpenGL: return CreateRef<OpenGL::GLPipeline>(config);
        }
        EK_ASSERT(false, "Shader creation not implemented for current graphics API");
        return nullptr;
    }
}
