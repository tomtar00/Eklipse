#include "precompiled.h"

#include "GL.h"
#include "GLImGuiAdapter.h"

#include <Eklipse/Renderer/RenderCommand.h>
#include <Eklipse/Platform/Windows/WindowsWindow.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>
#include <Eklipse/Core/Application.h>

#include <Eklipse/Assets/AssetManager.h>

namespace Eklipse
{
    namespace OpenGL
    {
        GLImGuiAdapter::GLImGuiAdapter(const ImGuiLayerConfig& config) : ImGuiAdapter(config)
        {
            m_glfwWindow = Application::Get().GetWindow()->GetGlfwWindow();
            EK_ASSERT(m_glfwWindow, "Failed to get GLFW window in GL ImGui Layer!");
        }
        void GLImGuiAdapter::Init()
        {
            EK_CORE_PROFILE();
            ImGui_ImplGlfw_InitForOpenGL(m_glfwWindow, true);
            ImGui_ImplOpenGL3_Init("#version 460");
        }
        void GLImGuiAdapter::Shutdown()
        {
            EK_CORE_PROFILE();
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
        }
        void GLImGuiAdapter::NewFrame()
        {
            EK_CORE_PROFILE();
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
        }
        void GLImGuiAdapter::Render()
        {
            EK_CORE_PROFILE();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }
        void GLImGuiAdapter::DrawViewport(Framebuffer* framebuffer, float width, float height)
        {
            EK_CORE_PROFILE();
            if (width != framebuffer->GetInfo().width || height != framebuffer->GetInfo().height)
            {
                ResizeViewport(framebuffer, width, height);
            }

            GLFramebuffer* glFramebuffer = static_cast<GLFramebuffer*>(framebuffer);
            ImGui::Image((ImTextureID)glFramebuffer->GetMainColorAttachment(), ImVec2{ width, height }, { 0, 1 }, { 1, 0 });
        }
        void GLImGuiAdapter::ResizeViewport(Framebuffer* framebuffer, float width, float height)
        {
            EK_CORE_PROFILE();
            if (width > 0 && height > 0)
                framebuffer->Resize(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
        }

        // =============== ICONS ===============

        GLImGuiIcon::GLImGuiIcon(const Path& texturePath)
        {
            EK_CORE_PROFILE();
            m_texture = CreateRef<GLTexture2D>(texturePath);
        }
        void* GLImGuiIcon::GetID()
        {
            EK_CORE_PROFILE();
            return (void*)m_texture->GetID();
        }
        void GLImGuiIcon::Dispose()
        {
            EK_CORE_PROFILE();
            m_texture->Dispose();
        }
    }
}