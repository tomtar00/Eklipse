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
			VulkanAPI(VkImGuiLayer* vkGui);
			~VulkanAPI() override;

			void Init(Scene* scene) override;
			void Shutdown() override;

			void DrawFrame() override;
			void OnPostLoop() override;

			float GetAspectRatio() override;

			static VulkanAPI& Get();

		private:
			void BeginComputeStage();
			void SubmitComputeStage();
			uint32_t BeginDrawStage();
			void SubmitDrawStage(uint32_t imageIndex);

			void CreateInstance();
			void CreateSurface();
			void CreateSyncObjects();

			void RecreateSwapChain();
			std::vector<const char*> GetRequiredExtensions() const;

		private:
			inline static VulkanAPI* s_instance = nullptr;
			VkImGuiLayer* m_vkGui;

			ModelManager m_modelManager;

			uint32_t m_currentFrameInFlightIndex;

			std::vector<VkSemaphore> m_imageAvailableSemaphores{};
			std::vector<VkSemaphore> m_renderFinishedSemaphores{};
			std::vector<VkFence> m_inFlightFences{};
			std::vector<VkFence> m_computeInFlightFences{};
			std::vector<VkSemaphore> m_computeFinishedSemaphores{};
		};
	}
}