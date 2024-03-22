#pragma once
#include "Texture.h"

namespace Eklipse
{
	enum class PresentMode
	{
        IMMEDIATE	= 0,
        MAILBOX		= 1,
        FIFO		= 2,
        FIFO_RELAXED = 3,
    };
	String EK_API PresentModeToString(PresentMode mode);
	PresentMode EK_API PresentModeFromString(const String& mode);

	struct FramebufferAttachmentInfo
	{
		ImageFormat textureFormat = ImageFormat::FORMAT_UNDEFINED;
	};
	struct FramebufferInfo
	{
		uint32_t width{ 1 }, height{ 1 };
		uint32_t numSamples{ 1 };
		Vec<FramebufferAttachmentInfo> colorAttachmentInfos;
		FramebufferAttachmentInfo depthAttachmentInfo;
		bool isDefaultFramebuffer{ false };
	};

	class EK_API Framebuffer
	{
	public:
		Framebuffer(const FramebufferInfo& framebufferInfo);
		static Ref<Framebuffer> Create(const FramebufferInfo& framebufferInfo);

		const float GetAspectRatio() const;
		virtual void Resize(uint32_t width, uint32_t height);

		virtual Ref<Texture2D> GetColorAttachment(uint32_t index) = 0;

		virtual FramebufferInfo& GetInfo() = 0;
		virtual void Build() = 0;
		virtual void Bind() = 0;
		virtual void Unbind() = 0;
		virtual void Dispose() = 0;

	protected:
		float m_aspectRatio;
	};

	extern EK_API Framebuffer* g_defaultFramebuffer;
	extern EK_API Framebuffer* g_currentFramebuffer;
	extern EK_API Vec<Framebuffer*> g_offScreenFramebuffers;
}