#pragma once

namespace Eklipse
{
	enum class FramebufferTextureFormat
	{
		None = 0,

		// Color
		RGBA8,
		RED_INTEGER,

		// Depth/stencil
		DEPTH24STENCIL8,
		Depth = DEPTH24STENCIL8
	};

	struct FramebufferAttachmentInfo
	{
		FramebufferTextureFormat textureFormat = FramebufferTextureFormat::None;
	};

	struct FramebufferInfo
	{
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

		inline const FramebufferInfo& GetInfo() const { return m_framebufferInfo; }

		virtual void Build() = 0;
		virtual void Bind() = 0;
		virtual void Unbind() = 0;
		virtual void Draw() = 0;
		virtual void Resize(uint32_t width, uint32_t height) = 0;

	protected:
		FramebufferInfo m_framebufferInfo;
	};
}