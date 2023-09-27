#pragma once
#include <Eklipse.h>

namespace Editor
{
	class ViewPanel : public Eklipse::Layer
	{
	public:
		virtual void OnGUI(float deltaTime) override;
		float GetAspectRatio() const { return m_aspectRatio; }

	private:
		float m_aspectRatio{ 1.0f };
	};
}