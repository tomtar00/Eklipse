#pragma once
#include <Eklipse.h>
#include <Eklipse/Assets/RuntimeAssetLibrary.h>

namespace Eklipse
{
	class RuntimeLayer : public Layer
	{
	public:
		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate(float deltaTime) override;
		void OnGUI(float deltaTime) override;

		void OnAPIHasInitialized(ApiType api);
		void OnShutdownAPI();

	private:
		Ref<RuntimeConfig> m_runtimeConfig;
		Ref<RuntimeAssetLibrary> m_runtimeAssetLibrary;
		Ref<ScriptLinker> m_scriptLinker;
	};
}