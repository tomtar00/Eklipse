#include "EditorLayer.h"
#include <glm/gtc/quaternion.hpp>
#include <Eklipse/Scene/Components.h>
#include <Eklipse/Renderer/Settings.h>

namespace Editor
{
	EditorLayer::EditorLayer(Eklipse::Scene& scene) : m_guiEnabled(true), m_activeScene(&scene)
	{
		EK_ASSERT(s_instance == nullptr, "Editor layer already exists!");
		s_instance = this;

		m_editorCamera.m_farPlane = 1000.0f;
		m_editorCamera.m_nearPlane = 0.1f;
		m_editorCamera.m_fov = 45.0f;
	}
	void EditorLayer::OnAttach()
	{
		// GUI Layer
		m_guiLayerCreateInfo.enabled = &m_guiEnabled;
		m_guiLayerCreateInfo.menuBarEnabled = true;
		m_guiLayerCreateInfo.dockingEnabled = true;
		m_guiLayerCreateInfo.dockLayouts =
		{
			{ "Entities",	ImGuiDir_Left,	Eklipse::Dir_Opposite,	0.20f },
			{ "Settings",	ImGuiDir_Down,	Eklipse::Dir_Same,		0.60f },
			{ "Stats",		ImGuiDir_Down,	Eklipse::Dir_Same,		0.50f },
			{ "Details",	ImGuiDir_Right,	Eklipse::Dir_Opposite,	0.25f },
			{ "Logs",		ImGuiDir_Down,	Eklipse::Dir_Opposite,	0.30f },
			{ "Profiler",	ImGuiDir_Down,	Eklipse::Dir_Stack,		1.00f },
			{ "View",		ImGuiDir_None,	Eklipse::Dir_Opposite,	0.50f }
		};
		m_guiLayerCreateInfo.panels =
		{
			&m_entitiesPanel,
			&m_settingsPanel,
			&m_statsPanel,
			&m_detailsPanel,
			&m_logsPanel,
			&m_profilerPanel,
			&m_viewPanel
		};

		EK_INFO("Editor layer attached");
	}
	void EditorLayer::OnDetach()
	{
		EK_INFO("Editor layer detached");
	}
	void EditorLayer::OnUpdate(float deltaTime)
	{
		EK_PROFILE_NAME("Editor");

		// =================================== CAMERA
		static float pitch = 0.0f;
		static float yaw = 0.0f;
		static float distance = 10.0f;
		if (Eklipse::Input::IsScrollingUp())
		{
			distance -= 0.5f;
		}
		if (Eklipse::Input::IsScrollingDown())
		{
			distance += 0.5f;
		}
		distance = glm::clamp(distance, 1.0f, 100.0f);

		static glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
		static glm::vec3 targetPosition = glm::vec3(0.0f, 0.0f, 0.0f);

		// TODO: check if input was started in view window

		if (Eklipse::Input::IsKeyDown(Eklipse::KeyCode::F))
		{
			if (!m_selectedEntity.IsNull())
				targetPosition = m_selectedEntity.GetComponent<Eklipse::TransformComponent>().transform.position;
		}
		else if (Eklipse::Input::IsMouseButtonDown(Eklipse::MouseCode::Button1))
		{
			float mouseXDelta = -Eklipse::Input::GetMouseDeltaX();
			float mouseYDelta = Eklipse::Input::GetMouseDeltaY();

			pitch -= mouseYDelta * deltaTime * 100.f;
			pitch = glm::clamp(pitch, -89.0f, 89.0f);
			yaw -= mouseXDelta * deltaTime * 100.f;		
		}
		else if (Eklipse::Input::IsMouseButtonDown(Eklipse::MouseCode::Button2))
		{
			float mouseXDelta = Eklipse::Input::GetMouseDeltaX();
			float mouseYDelta = -Eklipse::Input::GetMouseDeltaY();

			glm::vec3 cameraDir = glm::normalize(targetPosition - m_editorCameraTransform.position);
			glm::vec3 cameraRight = glm::normalize(glm::cross(cameraDir, cameraUp));
			targetPosition += (mouseXDelta * cameraRight + mouseYDelta * cameraUp) * deltaTime * 5.f;
		}

		glm::vec3 cameraPosition{};
		cameraPosition.x = distance * cos(glm::radians(pitch)) * sin(glm::radians(yaw));
		cameraPosition.y = distance * sin(glm::radians(pitch));
		cameraPosition.z = distance * cos(glm::radians(pitch)) * cos(glm::radians(yaw));
		cameraPosition += targetPosition;

		m_editorCameraTransform.position = cameraPosition;
		glm::vec3 cameraDir = glm::normalize(targetPosition - m_editorCameraTransform.position);
		m_editorCameraTransform.rotation = glm::degrees(-glm::eulerAngles(glm::quatLookAt(cameraDir, cameraUp)));	
		// ===================================
	}
	void EditorLayer::Render(Eklipse::Scene& scene, float deltaTime)
	{
		EK_PROFILE();

		m_activeScene = &scene;
		
		Eklipse::Renderer::BeginFrame(m_editorCamera, m_editorCameraTransform);

		// Record scene framebuffer
		Eklipse::Renderer::BeginRenderPass(m_viewportFramebuffer);
		Eklipse::Renderer::RenderScene(*m_activeScene);
		Eklipse::Renderer::EndRenderPass(m_viewportFramebuffer);

		// Record ImGui framebuffer
		Eklipse::Renderer::BeginRenderPass(m_defaultFramebuffer);
		GUI->Render();
		Eklipse::Renderer::EndRenderPass(m_defaultFramebuffer);

		Eklipse::Renderer::Submit();
	}
	void EditorLayer::OnAPIHasInitialized(Eklipse::ApiType api)
	{
		// Create default framebuffer (for ImGui)
		{
			Eklipse::FramebufferInfo fbInfo{};
			fbInfo.framebufferType			= Eklipse::FramebufferType::DEFAULT;
			fbInfo.width					= Eklipse::Application::Get().GetInfo().windowWidth;
			fbInfo.height					= Eklipse::Application::Get().GetInfo().windowHeight;
			fbInfo.numSamples				= 1;
			fbInfo.colorAttachmentInfos		= { { Eklipse::ImageFormat::RGBA8 } };
			fbInfo.depthAttachmentInfo		= { Eklipse::ImageFormat::FORMAT_UNDEFINED };

			m_defaultFramebuffer = Eklipse::Framebuffer::Create(fbInfo);
		}

		// Create off-screen framebuffer (for scene view)
		{
			Eklipse::FramebufferInfo fbInfo{};
			fbInfo.framebufferType			= Eklipse::FramebufferType::SCENE_VIEW;
			fbInfo.width					= GetViewPanel().GetViewportSize().x > 0 ? GetViewPanel().GetViewportSize().x : 512;
			fbInfo.height					= GetViewPanel().GetViewportSize().y > 0 ? GetViewPanel().GetViewportSize().y : 512;
			fbInfo.numSamples				= Eklipse::RendererSettings::GetMsaaSamples();
			fbInfo.colorAttachmentInfos		= { { Eklipse::ImageFormat::RGBA8 } };
			fbInfo.depthAttachmentInfo		= { Eklipse::ImageFormat::D24S8 };

			m_viewportFramebuffer = Eklipse::Framebuffer::Create(fbInfo);
		}

		GUI.reset();
		GUI = Eklipse::ImGuiLayer::Create(GetGuiInfo());
		Eklipse::Application::Get().PushOverlay(GUI);
		GUI->Init();
	}
	void EditorLayer::OnShutdownAPI()
	{
		Eklipse::Application::Get().PopOverlay(GUI);
		SetEntityNull();
		GUI->Shutdown();
		m_defaultFramebuffer->Dispose();
		m_viewportFramebuffer->Dispose();

		m_defaultFramebuffer.reset();
		m_viewportFramebuffer.reset();
	}
}