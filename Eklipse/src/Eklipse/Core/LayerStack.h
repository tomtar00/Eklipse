#pragma once
#include "Eklipse/Core/Layer.h"
#include <vector>

namespace Eklipse 
{
	class EK_API LayerStack
	{
	public:
		LayerStack() = default;
		~LayerStack() {};

		void PushLayer(Ref<Layer> layer);
		void PopLayer(Ref<Layer> layer);

		void PushOverlay(Ref<Layer> overlay);
		void PopOverlay(Ref<Layer> overlay);

		void Shutdown();

		Vec<Ref<Layer>>::iterator begin() { return m_layers.begin(); }
		Vec<Ref<Layer>>::iterator end() { return m_layers.end(); }

	private:
		Vec<Ref<Layer>> m_layers;
		uint32_t m_lastLayerIndex = 0;
	};
}