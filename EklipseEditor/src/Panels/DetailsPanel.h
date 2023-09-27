#pragma once
#include <Eklipse.h>
#include <Eklipse/Scene/Entity.h>

namespace Editor
{
	class DetailsPanel : public Eklipse::Layer
	{
	public:
		void Setup(Eklipse::Entity entity, std::string& name);
		inline void SetEntityDeleted(bool deleted) { m_entityDeleted = deleted; }
		
	private:
		virtual void OnGUI(float deltaTime) override;

	private:
		Eklipse::Entity m_entity;
		std::string m_entityNameBuffer;
		bool m_entityDeleted = false;
	};
}