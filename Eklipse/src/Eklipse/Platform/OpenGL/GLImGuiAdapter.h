#pragma once
#include <Eklipse/ImGui/ImGuiLayer.h>
#include <GLFW/glfw3.h>
#include <Eklipse/Platform/OpenGL/GLTexture.h>

namespace Eklipse
{
    namespace OpenGL
    {
        class GLImGuiIcon : public GuiIcon
        {
        public:
            GLImGuiIcon(const Path& texturePath);
            virtual void* GetID() override;
            virtual void Dispose() override;

        private:
            Ref<GLTexture2D> m_texture;
        };

        class GLImGuiAdapter : public ImGuiAdapter
        {
        public:
            GLImGuiAdapter(const ImGuiLayerConfig& config);

            virtual void Init() override;
            virtual void Shutdown() override;
            virtual void NewFrame() override;
            virtual void Render() override;

            virtual void DrawViewport(Framebuffer* framebuffer, float width, float height) override;
            virtual void ResizeViewport(Framebuffer* framebuffer, float width, float height) override;

        private:
            GLFWwindow* m_glfwWindow;
        };
    }
}