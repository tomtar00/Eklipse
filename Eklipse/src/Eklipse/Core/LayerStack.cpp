#include "precompiled.h"
#include "LayerStack.h"
#include "Layer.h"

namespace Eklipse 
{
	LayerStack::~LayerStack()
	{
		for (Layer* layer : m_layers)
		{
			layer->OnDetach();
			delete layer;
		}
	}

	void LayerStack::PushLayer(Layer* layer)
	{
		layer->OnAttach();
		m_layers.emplace_back(layer);
	}

	void LayerStack::PopLayer(Layer* layer)
	{
		auto it = std::find(m_layers.begin(), m_layers.end(), layer);
		if (it != m_layers.end())
		{
			layer->OnDetach();
			m_layers.erase(it);
		}
	}
}