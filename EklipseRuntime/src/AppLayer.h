#pragma once
#include <Eklipse.h>
#include <dylib.hpp>

namespace Eklipse
{
	class AppLayer : public Layer
	{
	public:
		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate(float deltaTime) override;
		void OnGUI(float deltaTime) override;

		void OnAPIHasInitialized(ApiType api);
		void OnShutdownAPI();

	private:
		Ref<Framebuffer> m_framebuffer;
		Ref<RuntimeConfig> m_runtimeConfig;
		Ref<dylib> m_library;
	};
}