#include "EditorLayer.h"
#include <Eklipse/Renderer/RenderCommand.h>
#include <glm/gtc/quaternion.hpp>
#include <Eklipse/Scene/Components.h>

namespace Editor
{
	EditorLayer::EditorLayer(Eklipse::Scene& scene) : m_guiEnabled(true), m_activeScene(&scene)
	{
		EK_ASSERT(s_instance == nullptr, "Editor layer already exists!");
		s_instance = this;

		m_entitiesPanel = Eklipse::CreateRef<EntitiesPanel>();
		m_settingsPanel = Eklipse::CreateRef<SettingsPanel>();
		m_statsPanel = Eklipse::CreateRef<StatsPanel>();
		m_detailsPanel = Eklipse::CreateRef<DetailsPanel>();
		m_logsPanel = Eklipse::CreateRef<LogsPanel>();
		m_profilerPanel = Eklipse::CreateRef<ProfilerPanel>();
		m_viewPanel = Eklipse::CreateRef<ViewPanel>();

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
			{ "Entities",	ImGuiDir_Left,	Eklipse::Dir_Opposite,	0.20f,  m_entitiesPanel	},
			{ "Settings",	ImGuiDir_Down,	Eklipse::Dir_Same,		0.60f,	m_settingsPanel	},
			{ "Stats",		ImGuiDir_Down,	Eklipse::Dir_Same,		0.50f,  m_statsPanel	},
			{ "Details",	ImGuiDir_Right,	Eklipse::Dir_Opposite,	0.25f,	m_detailsPanel	},
			{ "Logs",		ImGuiDir_Down,	Eklipse::Dir_Opposite,	0.30f,	m_logsPanel		},
			{ "Profiler",	ImGuiDir_Down,	Eklipse::Dir_Stack,		1.00f,	m_profilerPanel	},
			{ "View",		ImGuiDir_None,	Eklipse::Dir_Opposite,	0.50f,	m_viewPanel		}
		};

		// TODO: Framebuffer

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

		m_editorCamera.UpdateViewProjectionMatrix(m_editorCameraTransform, m_viewPanel->GetAspectRatio());
		// ===================================
	}
	void EditorLayer::Render(Eklipse::Scene& scene, float deltaTime)
	{
		m_activeScene = &scene;

		// Rendering
		Eklipse::Renderer::RecordViewport(scene, m_editorCamera, deltaTime);

		// Drawing
		Eklipse::RenderCommand::API->BeginFrame();
		GUI->Draw();
		Eklipse::RenderCommand::API->EndFrame();
	}
}