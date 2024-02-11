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
		EK_CORE_PROFILE();
		layer->OnAttach();
		m_layers.emplace(m_layers.begin() + m_lastLayerIndex, layer);
		m_lastLayerIndex++;
	}
	void LayerStack::PopLayer(Ref<Layer> layer)
	{
		EK_CORE_PROFILE();
		auto it = std::find(m_layers.begin(), m_layers.begin() + m_lastLayerIndex, layer);
		if (it != m_layers.begin() + m_lastLayerIndex)
		{
			layer->OnDetach();
			m_layers.erase(it);
			m_lastLayerIndex--;
		}
	}

	void LayerStack::PushOverlay(Ref<Layer> overlay)
	{
		EK_CORE_PROFILE();
		overlay->OnAttach();
		m_layers.emplace_back(overlay);
	}

	void LayerStack::PopOverlay(Ref<Layer> overlay)
	{
		EK_CORE_PROFILE();

		uint32_t index = 0;
		for (auto& layer : *this)
		{
			if (&*layer.get() == &*overlay.get())
			{
				overlay->OnDetach();
				m_layers.erase(m_layers.begin() + index);
				break;
			}
			index++;
		}
	}

	void LayerStack::Shutdown()
	{
		EK_CORE_PROFILE();
		for (auto& layer : *this)
		{
			layer->OnDetach();
		}
	}
}