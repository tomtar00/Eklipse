#pragma once
#include <Eklipse.h>

namespace Editor
{
	class ProfilerPanel : public Eklipse::Layer
	{
	public:
		virtual void OnGUI(float deltaTime) override;
	private:
		bool m_ascendingSort = false;
		uint32_t m_columnIndex = -1;
	};
}