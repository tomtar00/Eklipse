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

			Vec<const char*> GetRequiredExtensions() const;

		private:
			Ref<VKFramebuffer> m_defaultFramebuffer;

			Vec<VkSemaphore> m_imageAvailableSemaphores{};
			Vec<VkSemaphore> m_renderFinishedSemaphores{};
			Vec<VkFence> m_renderInFlightFences{};

			Vec<VkFence> m_computeInFlightFences{};
			Vec<VkSemaphore> m_computeFinishedSemaphores{};
		};
	}
}