#pragma once
#include "Texture.h"

namespace Eklipse
{
	enum FramebufferType
	{
		DEFAULT		= BIT(0),
		SCENE_VIEW	= BIT(1),
		OFFSCREEN	= BIT(2)
	};
	struct FramebufferAttachmentInfo
	{
		ImageFormat textureFormat = ImageFormat::FORMAT_UNDEFINED;
	};
	struct FramebufferInfo
	{
		FramebufferType framebufferType{ FramebufferType::DEFAULT };
		uint32_t width{ 1 }, height{ 1 };
		uint32_t numSamples{ 1 };
		std::vector<FramebufferAttachmentInfo> colorAttachmentInfos;
		FramebufferAttachmentInfo depthAttachmentInfo;
	};

	class Framebuffer
	{
	public:
		static Ref<Framebuffer> Create(const FramebufferInfo& framebufferInfo);
		virtual ~Framebuffer() = default;

		virtual FramebufferInfo& GetInfo() = 0;

		virtual void Build() = 0;
		virtual void Bind() = 0;
		virtual void Unbind() = 0;
		virtual void Resize(uint32_t width, uint32_t height);
		virtual void Dispose() = 0;

		inline const float GetAspectRatio() const { return m_aspectRatio; }

	protected:
		float m_aspectRatio;
	};

	extern Framebuffer* g_defaultFramebuffer;
	extern Framebuffer* g_sceneFramebuffer;
}