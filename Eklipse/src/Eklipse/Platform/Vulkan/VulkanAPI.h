#pragma once
#include <Eklipse/Renderer/GraphicsAPI.h>
#include <vulkan/vulkan.h>
#include "VkModel.h"
#include "VkImGuiLayer.h"

namespace Eklipse
{
	namespace Vulkan
	{
		class VulkanAPI : public GraphicsAPI
		{
		public:
			VulkanAPI();
			~VulkanAPI() override;

			void Init(Scene* scene) override;
			void Shutdown() override;

			void DrawFrame() override;
			void DrawGUI() override;

			void OnPostLoop() override;

			float GetAspectRatio() override;

			static VulkanAPI& Get();

		private:
			void CreateInstance();
			void CreateSurface();
			void CreateSyncObjects();

			void RecreateSwapChain();
			std::vector<const char*> GetRequiredExtensions() const;

		private:
			inline static VulkanAPI* s_instance = nullptr;

			ModelManager m_modelManager;

			std::vector<VkSemaphore> m_imageAvailableSemaphores{};
			std::vector<VkSemaphore> m_renderFinishedSemaphores{};
			std::vector<VkFence> m_renderInFlightFences{};

			std::vector<VkFence> m_computeInFlightFences{};
			std::vector<VkSemaphore> m_computeFinishedSemaphores{};
		};
	}
}