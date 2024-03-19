#pragma once
#include <map>
#include "Shader.h"
#include "Framebuffer.h"

#include <Eklipse/Core/Timer.h>

#define EK_PIPELINE_LIFETIME 5000.0f

namespace Eklipse
{
	class EK_API Pipeline
	{
	public:
		enum class Type
		{
			Resterization,
			RayTracing,
			RayTracingKHR,
			Compute
		};
		enum class TopologyMode
		{
			Triangle,
			Line
		};
		struct Config
		{
            Type type;
            TopologyMode topologyMode;
			Shader* shader;
			Framebuffer* framebuffer;
        };

	public:
		Pipeline(const Config& config);

		static Ref<Pipeline> Get(const Config& config);
		static Vec<Ref<Pipeline>> GetPipelinesByShader(const AssetHandle shaderHandle);

		static String TypeToString(const Type& type);
		static String TopologyModeToString(const TopologyMode& topologyMode);
		static Type StringToType(const String& type);
		static TopologyMode StringToTopologyMode(const String& topologyMode);
		static void DeleteUnsused();
		static void DisposeAll();

		virtual void Build() = 0;
		virtual void Bind() = 0;
		virtual void Dispose() = 0;

	private:
        static Ref<Pipeline> Create(const Config& config);
		static std::map<size_t, Ref<Pipeline>> s_pipelines;

	protected:
		Config m_config;
		TimePoint m_lastAccessTime;
	};
}