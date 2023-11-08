#include "precompiled.h"

#include "GL.h"
#include "GLImGuiLayer.h"

#include <Eklipse/Renderer/RenderCommand.h>
#include <Eklipse/Platform/Windows/WindowsWindow.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>
#include <Eklipse/Core/Application.h>

namespace Eklipse
{
	namespace OpenGL
	{
		GLImGuiLayer::GLImGuiLayer(const GuiLayerConfigInfo& configInfo) : Eklipse::ImGuiLayer(configInfo)
		{
			m_glfwWindow = Application::Get().GetWindow()->GetGlfwWindow();
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
			ImGuiLayer::Shutdown();
		}
		void GLImGuiLayer::NewFrame()
		{
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
		}
		void GLImGuiLayer::Render()
		{
			if (!(*m_config.enabled)) return;

			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}
		void GLImGuiLayer::DrawViewport(float width, float height)
		{
			if (width != g_GLSceneFramebuffer->GetInfo().width || height != g_GLSceneFramebuffer->GetInfo().height)
			{
				ResizeViewport(width, height);
			}

			ImGui::Image((ImTextureID)g_GLSceneFramebuffer->GetMainColorAttachment(), ImVec2{ width, height }, { 0, 1 }, { 1, 0 });
		}
		void GLImGuiLayer::ResizeViewport(float width, float height)
		{
			if (width > 0 && height > 0)
				g_GLSceneFramebuffer->Resize(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
		}

		// =============== ICONS ===============

		GLImGuiIcon::GLImGuiIcon(const Ref<AssetLibrary> assetLibrary, const Path& path)
		{
			m_texture = std::static_pointer_cast<GLTexture2D>(assetLibrary->GetTexture(path));
		}
		void* GLImGuiIcon::GetID()
		{
			return (void*)m_texture->GetID();
		}
	}
}