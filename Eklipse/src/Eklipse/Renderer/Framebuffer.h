#pragma once
#include "Texture.h"

namespace Eklipse
{
	struct FramebufferAttachmentInfo
	{
		ImageFormat textureFormat = ImageFormat::FORMAT_UNDEFINED;
	};
	struct FramebufferInfo
	{
		uint32_t width{ 1 }, height{ 1 };
		uint32_t numSamples{ 1 };
		std::vector<FramebufferAttachmentInfo> colorAttachmentInfos;
		FramebufferAttachmentInfo depthAttachmentInfo;
		bool isDefaultFramebuffer{ false };
	};

	class Framebuffer
	{
	public:
		Framebuffer(const FramebufferInfo& framebufferInfo);
		static Ref<Framebuffer> Create(const FramebufferInfo& framebufferInfo);

		const float GetAspectRatio() const;
		virtual void Resize(uint32_t width, uint32_t height);

		virtual FramebufferInfo& GetInfo() = 0;
		virtual void Build() = 0;
		virtual void Bind() = 0;
		virtual void Unbind() = 0;
		virtual void Dispose() = 0;

	protected:
		float m_aspectRatio;
	};

	extern Framebuffer* g_defaultFramebuffer;
	extern Framebuffer* g_currentFramebuffer;
	extern std::vector<Framebuffer*> g_offScreenFramebuffers;
}