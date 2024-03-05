#pragma once

#include <Eklipse/Events/Event.h>
#include <Eklipse/Renderer/GraphicsAPI.h>

namespace Eklipse
{
	class EK_API Layer
	{
	public:
		Layer() = default;
		virtual ~Layer() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}

		virtual void OnEvent(Event& event) {}

		virtual void OnUpdate(float deltaTime) {}
		virtual void OnRender(float deltaTime) {}
		virtual void OnGUI(float deltaTime) {}
		virtual void OnCompute(float deltaTime) {}

		virtual void OnAPIHasInitialized(GraphicsAPI::Type api) {}
		virtual void OnShutdownAPI(bool quit) {}
	};
}