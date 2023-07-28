#pragma once

#include "Eklipse/Events/Event.h"

namespace Eklipse
{
	class EK_API Layer
	{
	public:
		Layer() = default;
		virtual ~Layer() {}

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnEvent(Event& event) {}

		virtual void Update(float deltaTime) {}
	};
}