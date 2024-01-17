#include "AppLayer.h"
#include <Eklipse/Project/Project.h>
#include <Eklipse/Project/ProjectSerializer.h>

#define EK_RUNTIME_CONFIG "config.yaml"

namespace Eklipse
{
	void AppLayer::OnAttach()
	{
		ProjectSerializer serializer;
		m_runtimeConfig = CreateRef<RuntimeConfig>();
		if (!serializer.DeserializeRuntimeConfig(*m_runtimeConfig, EK_RUNTIME_CONFIG))
		{
			EK_CRITICAL("Failed to deserialize runtime config! Make sure that the '{}' file is in the same folder as the executable file", EK_RUNTIME_CONFIG);
			exit(-1);
		}
		Project::SetRuntimeConfig(m_runtimeConfig);

		try
		{
			if (!m_runtimeConfig->scriptsLibraryPath.empty())
			{
				m_library = CreateRef<dylib>(m_runtimeConfig->scriptsLibraryPath);
			}
			else
			{
				EK_INFO("Scripts library not found. No scripts will be attached");
			}
			auto scene = Scene::Load(m_runtimeConfig->startScenePath, m_library);
			Application::Get().SwitchScene(scene);
		}
		catch (const std::exception& e)
		{
			EK_CRITICAL("Failed to load scene! {0}", e.what());
			exit(-1);
		}

		Application::Get().GetActiveScene()->OnSceneStart();
	}
	void AppLayer::OnDetach()
	{
		ProjectSerializer serializer;
		if (!serializer.SerializeRuntimeConfig(*m_runtimeConfig, EK_RUNTIME_CONFIG))
		{
			EK_ERROR("Failed to serialize runtime config!");
		}
	}

	void AppLayer::OnUpdate(float deltaTime)
	{
		Application::Get().GetActiveScene()->OnSceneUpdate(deltaTime);

		Renderer::BeginRenderPass(m_framebuffer);
		Renderer::RenderScene(Application::Get().GetActiveScene());
		Renderer::EndRenderPass(m_framebuffer);
	}
	void AppLayer::OnGUI(float deltaTime)
	{
		// TODO: Terminal Panel (ImGui)
	}

	void AppLayer::OnAPIHasInitialized(ApiType api)
	{
		FramebufferInfo fbInfo{};
		fbInfo.framebufferType		= FramebufferType::DEFAULT;
		fbInfo.width				= Application::Get().GetInfo().windowWidth;
		fbInfo.height				= Application::Get().GetInfo().windowHeight;
		fbInfo.numSamples			= Renderer::GetSettings().GetMsaaSamples();
		fbInfo.colorAttachmentInfos = { { ImageFormat::RGBA8 } };
		fbInfo.depthAttachmentInfo	= { ImageFormat::D24S8 };

		m_framebuffer = Framebuffer::Create(fbInfo);
	}
	void AppLayer::OnShutdownAPI()
	{
		m_framebuffer->Dispose();
		m_framebuffer.reset();
	}
}