#pragma once
#include <Eklipse/Renderer/GraphicsAPI.h>
#include <vulkan/vulkan.h>
#include "VKImGuiLayer.h"
#include "VKFramebuffer.h"

namespace Eklipse
{
	namespace Vulkan
	{
		class VulkanAPI : public GraphicsAPI
		{
		public:
			VulkanAPI();

			bool Init() override;
			void Shutdown() override;
			void WaitDeviceIdle() override;

			void BeginFrame() override;
			void EndFrame() override;

			void DrawIndexed(Ref<VertexArray> vertexArray) override;

		private:
			void CreateInstance();
			void CreateAllocator();
			void CreateSurface();
			void CreateDefaultFramebuffer();
			void CreateSyncObjects();

			std::vector<const char*> GetRequiredExtensions() const;

		private:
			Ref<VKFramebuffer> m_defaultFramebuffer;

			std::vector<VkSemaphore> m_imageAvailableSemaphores{};
			std::vector<VkSemaphore> m_renderFinishedSemaphores{};
			std::vector<VkFence> m_renderInFlightFences{};

			std::vector<VkFence> m_computeInFlightFences{};
			std::vector<VkSemaphore> m_computeFinishedSemaphores{};
		};
	}
}