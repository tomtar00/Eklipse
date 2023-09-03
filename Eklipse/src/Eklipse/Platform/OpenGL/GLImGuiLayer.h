#pragma once
#include <Eklipse/ImGui/ImGuiLayer.h>
#include <GLFW/glfw3.h>

namespace Eklipse
{
	namespace OpenGL
	{
		class GLImGuiLayer : public Eklipse::ImGuiLayer
		{
		public:
			GLImGuiLayer(Window* window, GuiLayerConfigInfo configInfo);
			virtual ~GLImGuiLayer() {}

			virtual void Init() override;
			virtual void Shutdown() override;
			virtual void NewFrame() override;
			virtual void Draw() override;

			virtual void DrawViewport(float width, float height) override;
			virtual void RecreateViewport(float width, float height) override;

		private:
			GLFWwindow* m_glfwWindow;
		};
	}
}