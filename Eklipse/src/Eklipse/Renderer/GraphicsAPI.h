#pragma once
#include "Pipeline.h"

namespace Eklipse
{
	enum class ApiType
	{
		None,
		Vulkan
	};

	class GraphicsAPI
	{
	public:
		GraphicsAPI();
		virtual ~GraphicsAPI() {};

		virtual void Init() {};
		virtual void Shutdown() {};

		virtual void DrawFrame() = 0;
		virtual void WaitIdle() = 0;

		bool IsInitialized();
		ApiType GetApiType();
		void SetApiType(ApiType apiType);

	protected:
		ApiType m_apiType;
		Pipeline m_pipeline;
		bool m_initialized = false;
	};
}