#pragma once
#include <Eklipse/Renderer/GraphicsAPI.h>
#include <vulkan/vulkan.h>
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
			void Submit() override;

			void DrawIndexed(Ref<VertexArray> vertexArray) override;

		private:
			void CreateInstance();
			void CreateAllocator();
			void CreateSurface();
			void CreateSyncObjects();

			Vec<const char*> GetRequiredExtensions() const;

		private:
			Vec<VkSemaphore> m_imageAvailableSemaphores{};

			Vec<VkFence> m_renderInFlightFences{};
			Vec<VkSemaphore> m_renderFinishedSemaphores{};

			Vec<VkFence> m_computeInFlightFences{};
			Vec<VkSemaphore> m_computeFinishedSemaphores{};
		};
	}
}