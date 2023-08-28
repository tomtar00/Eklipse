#include "precompiled.h"

#include "GL.h"
#include "GLImGuiLayer.h"
#include <Eklipse/Platform/Windows/WindowsWindow.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>

namespace Eklipse
{
	namespace OpenGL
	{
		ViewportSize	g_viewportSize = { 512, 512 };
		void* g_viewportTexture;

		GLImGuiLayer::GLImGuiLayer(Window* window, GuiLayerConfigInfo configInfo) : Eklipse::ImGuiLayer(window, configInfo)
		{
			m_glfwWindow = dynamic_cast<WindowsWindow*>(window)->GetGlfwWindow();
			EK_ASSERT(m_glfwWindow, "Failed to get GLFW window in GL ImGui Layer!");
		}
		void GLImGuiLayer::Init()
		{
			if (s_initialized) return;
			s_initialized = true;

			ImGui_ImplGlfw_InitForOpenGL(m_glfwWindow, true);
			ImGui_ImplOpenGL3_Init("#version 430");
		}
		void GLImGuiLayer::Shutdown()
		{
			if (!s_initialized) return;
			s_initialized = false;

			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplGlfw_Shutdown();
		}
		void GLImGuiLayer::NewFrame()
		{
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
		}
		void GLImGuiLayer::Draw(void* data)
		{
			if (!(*m_config.enabled)) return;

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}
		void GLImGuiLayer::DrawViewport(float width, float height)
		{
			if (width != g_viewportSize.width || height != g_viewportSize.height)
			{
				RecreateViewport(width, height);
			}

			ImGui::Image(g_viewportTexture, ImVec2{ width, height });
		}
		void GLImGuiLayer::RecreateViewport(float width, float height)
		{
			// destroy viewport
			g_viewportSize = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
			// create viewport
		}
	}
}