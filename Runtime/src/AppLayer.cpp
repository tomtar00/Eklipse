#include "AppLayer.h"

namespace Eklipse
{
	void AppLayer::OnAttach()
	{
		// load all assets
		// compile shaders? or copy cache and load from there
		// load startup scene
		// link to scripting dll (it will be in the same dir, so really neccessery?)
	}
	void AppLayer::OnDetach()
	{
	}

	void AppLayer::OnUpdate(float deltaTime)
	{
		Renderer::BeginRenderPass(m_framebuffer);

		// Renderer::RenderScene(Application::Get().GetActiveScene());

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
		fbInfo.numSamples			= RendererSettings::GetMsaaSamples();
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