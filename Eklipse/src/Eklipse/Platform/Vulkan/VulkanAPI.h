#pragma once
#include <Eklipse/Renderer/GraphicsAPI.h>
#include <vulkan/vulkan.h>
#include "VKImGuiLayer.h"

namespace Eklipse
{
	namespace Vulkan
	{
		class VulkanAPI : public GraphicsAPI
		{
		public:
			VulkanAPI();
			static VulkanAPI& Get();

			void Init() override;
			void Shutdown() override;

			void BeginFrame() override;
			void EndFrame() override;
			
			virtual void BeginGeometryPass() override;
			virtual void BeginGUIPass() override;
			virtual void EndPass() override;

			virtual void DrawIndexed(Ref<VertexArray> vertexArray) override;

		private:
			void CreateInstance();
			void CreateSurface();
			void CreateSyncObjects();

			void RecreateSwapChain();
			std::vector<const char*> GetRequiredExtensions() const;

		private:
			inline static VulkanAPI* s_instance = nullptr;

			VkCommandBuffer m_viewportCommandBuffer;
			VkCommandBuffer m_imguiCommandBuffer;

			std::vector<VkSemaphore> m_imageAvailableSemaphores{};
			std::vector<VkSemaphore> m_renderFinishedSemaphores{};
			std::vector<VkFence> m_renderInFlightFences{};

			std::vector<VkFence> m_computeInFlightFences{};
			std::vector<VkSemaphore> m_computeFinishedSemaphores{};
		};
	}
}