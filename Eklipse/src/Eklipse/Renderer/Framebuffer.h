#pragma once
#include "Texture.h"

namespace Eklipse
{
	enum class FramebufferType
	{
		DEFAULT,
		OFFSCREEN
	};
	struct FramebufferAttachmentInfo
	{
		ImageFormat textureFormat = ImageFormat::UNDEFINED;
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

		virtual const FramebufferInfo& GetInfo() const = 0;

		virtual void Build() = 0;
		virtual void Bind() = 0;
		virtual void Unbind() = 0;
		virtual void Resize(uint32_t width, uint32_t height) = 0;
	};
}