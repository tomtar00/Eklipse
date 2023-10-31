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
			GLImGuiIcon(const char* path);
			virtual void* GetID() override;

		private:
			Ref<GLTexture2D> m_texture;
		};

		class GLImGuiLayer : public Eklipse::ImGuiLayer
		{
		public:
			GLImGuiLayer(const GuiLayerConfigInfo& configInfo);
			virtual ~GLImGuiLayer() {}

			virtual void Init() override;
			virtual void Shutdown() override;
			virtual void NewFrame() override;
			virtual void Render() override;

			virtual void DrawViewport(float width, float height) override;
			virtual void ResizeViewport(float width, float height) override;

		private:
			GLFWwindow* m_glfwWindow;
		};
	}
}