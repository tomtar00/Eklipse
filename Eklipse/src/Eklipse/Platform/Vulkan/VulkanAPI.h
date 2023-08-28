#pragma once
#include <Eklipse/Renderer/GraphicsAPI.h>
#include <vulkan/vulkan.h>
#include "VKImGuiLayer.h"
#include "VKEntity.h"

namespace Eklipse
{
	namespace Vulkan
	{
		class VulkanAPI : public GraphicsAPI
		{
		public:
			VulkanAPI();
			static VulkanAPI& Get();

			void Init(Scene* scene) override;
			void Shutdown() override;

			void BeginFrame() override;
			void EndFrame() override;
			void DrawFrame() override;

			float GetAspectRatio() override;
		private:
			void CreateInstance();
			void CreateSurface();
			void CreateSyncObjects();

			void RecreateSwapChain();
			std::vector<const char*> GetRequiredExtensions() const;

		private:
			inline static VulkanAPI* s_instance = nullptr;

			VkEntityManager m_entityManager;

			VkCommandBuffer viewportCommandBuffer;
			VkCommandBuffer imguiCommandBuffer;

			std::vector<VkSemaphore> m_imageAvailableSemaphores{};
			std::vector<VkSemaphore> m_renderFinishedSemaphores{};
			std::vector<VkFence> m_renderInFlightFences{};

			std::vector<VkFence> m_computeInFlightFences{};
			std::vector<VkSemaphore> m_computeFinishedSemaphores{};
		};
	}
}