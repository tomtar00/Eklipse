#pragma once
#include "Eklipse/Core/Layer.h"
#include <vector>

namespace Eklipse 
{
	class LayerStack
	{
	public:
		LayerStack() = default;
		~LayerStack() {};

		void PushLayer(Ref<Layer> layer);
		void PopLayer(Ref<Layer> layer);
		void Shutdown();

		std::vector<Ref<Layer>>::iterator begin() { return m_layers.begin(); }
		std::vector<Ref<Layer>>::iterator end() { return m_layers.end(); }

	private:
		std::vector<Ref<Layer>> m_layers;
	};
}