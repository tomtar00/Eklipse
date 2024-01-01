#pragma once
#include <Eklipse.h>

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
	};
}