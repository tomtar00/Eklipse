#pragma once
#include <Eklipse.h>
#include <Eklipse/Assets/RuntimeAssetLibrary.h>

namespace Eklipse
{
	class RuntimeLayer : public Layer
	{
	public:
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(float deltaTime) override;
		virtual void OnGUI(float deltaTime) override;
		virtual void OnRender(float deltaTime) override;

		virtual void OnAPIHasInitialized(GraphicsAPI::Type api) override;
		virtual void OnShutdownAPI(bool quit) override;

	private:
		Ref<RuntimeConfig> m_runtimeConfig;
		Ref<RuntimeAssetLibrary> m_runtimeAssetLibrary;
		Ref<ScriptLinker> m_scriptLinker;
	};
}