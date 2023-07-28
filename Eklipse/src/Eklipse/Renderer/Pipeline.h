#pragma once

namespace Eklipse
{
	enum class PipelineType
	{
		None,
		Resterization,
		RayTracing,
		PathTracing
	};

	class Pipeline
	{
	public:
		virtual ~Pipeline() {};

		virtual void InitPool() {};
		virtual void Shutdown() {};

		PipelineType GetPipelineType() { return m_pipelineType; }
		void SetPipelineType(PipelineType pipelineTye) { m_pipelineType = pipelineTye; }

	protected:
		PipelineType m_pipelineType;
		bool m_initialized = false;
	};
}