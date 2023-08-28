#include "precompiled.h"
#include "LayerStack.h"
#include "Layer.h"

namespace Eklipse 
{
	bool operator==(const Ref<Layer>& lhs, const Ref<Layer>& rhs)
	{
		return lhs == rhs;
	}

	void LayerStack::PushLayer(Ref<Layer> layer)
	{
		layer->OnAttach();
		m_layers.emplace_back(layer);
	}

	void LayerStack::PopLayer(Ref<Layer> layer)
	{
		auto it = std::find(m_layers.begin(), m_layers.end(), layer);
		if (it != m_layers.end())
		{
			layer->OnDetach();
			m_layers.erase(it);
		}
	}
	void LayerStack::Shutdown()
	{
		for (auto& layer : *this)
		{
			layer->OnDetach();
		}
	}
}