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
			GLImGuiLayer(const GuiLayerConfigInfo& configInfo);
			virtual ~GLImGuiLayer() {}

			virtual void Init() override;
			virtual void Shutdown() override;
			virtual void NewFrame() override;
			virtual void Draw() override;

			virtual void DrawViewport(float width, float height) override;
			virtual void ResizeViewport(float width, float height) override;

		private:
			GLFWwindow* m_glfwWindow;
		};
	}
}