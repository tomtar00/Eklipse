#pragma once
#include "Shader.h"
#include "Framebuffer.h"

namespace Eklipse
{
	class EK_API Pipeline
	{
	public:
		enum class Type
		{
			Resterization,
			Compute,
			RayTracing,
		};
		enum class Mode
		{
			TRIANGLE,
			LINE,
		};
		struct Config
		{
            Type type;
            Mode mode;
			Ref<Shader> shader;
			Ref<Framebuffer> framebuffer;
        };

	public:
		Pipeline(const Config& config);

		static Ref<Pipeline> Get(const Config& config);
		static Vec<Ref<Pipeline>> GetPipelinesByShader(const AssetHandle shaderHandle);

		virtual void Build() = 0;
		virtual void Bind() = 0;
		virtual void Dispose() = 0;

	private:
        static Ref<Pipeline> Create(const Config& config);
		static std::map<size_t, Ref<Pipeline>> s_pipelines;

	protected:
		Config m_config;
	};
}