#include "EditorLayer.h"

namespace Editor
{
	EditorLayer::EditorLayer() : m_guiEnabled(true)
	{
		EK_ASSERT(s_instance == nullptr, "Editor layer already exists!");
		s_instance = this;

		m_entitiesPanel	= Eklipse::CreateRef<EntitiesPanel>();
		m_settingsPanel	= Eklipse::CreateRef<SettingsPanel>();
		m_statsPanel	= Eklipse::CreateRef<StatsPanel>();
		m_detailsPanel	= Eklipse::CreateRef<DetailsPanel>();
		m_logsPanel		= Eklipse::CreateRef<LogsPanel>();
		m_profilerPanel	= Eklipse::CreateRef<ProfilerPanel>();
		m_viewPanel		= Eklipse::CreateRef<ViewPanel>();

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

		if (Eklipse::Input::IsKeyDown(Eklipse::KeyCode::W))
		{
			m_editorCameraTransform.position.z += deltaTime * 10.0f;
		}
		if (Eklipse::Input::IsKeyDown(Eklipse::KeyCode::S))
		{
			m_editorCameraTransform.position.z -= deltaTime * 10.0f;
		}
		if (Eklipse::Input::IsKeyDown(Eklipse::KeyCode::A))
		{
			m_editorCameraTransform.position.x -= deltaTime * 10.0f;
		}
		if (Eklipse::Input::IsKeyDown(Eklipse::KeyCode::D))
		{
			m_editorCameraTransform.position.x += deltaTime * 10.0f;
		}
		if (Eklipse::Input::IsKeyDown(Eklipse::KeyCode::Space))
		{
			m_editorCameraTransform.position.y += deltaTime * 10.0f;
		}
		if (Eklipse::Input::IsKeyDown(Eklipse::KeyCode::LeftShift))
		{
			m_editorCameraTransform.position.y -= deltaTime * 10.0f;
		}

		m_editorCamera.UpdateViewProjectionMatrix(m_editorCameraTransform, m_viewPanel->GetAspectRatio());
		Eklipse::Renderer::DrawFrame(m_editorCamera, deltaTime);
	}
}