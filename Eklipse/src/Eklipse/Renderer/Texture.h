#pragma once

namespace Eklipse
{
	struct TextureData // TODO: remove
	{
		int width, height;
		void* pixels;
	};
	enum ImageFormat
	{
		FORMAT_UNDEFINED	= 0,
		R8					= BIT(1),
		RGB8				= BIT(2),
		RGBA8				= BIT(3),
		BGRA8				= BIT(4),
		RGBA32F				= BIT(5),
		D24S8				= BIT(6),
	};
	enum ImageAspect
	{
		ASPECT_UNDEFINED	= 0,
		COLOR				= BIT(1),
		DEPTH				= BIT(2),
		STENCIL				= BIT(3),
	};
	enum ImageUsage
	{
		USAGE_UNDEFINED		= 0,
		SAMPLED				= BIT(1),
		COLOR_ATTACHMENT	= BIT(2),
		DEPTH_ATTACHMENT	= BIT(3),
		TRANSFER_SRC		= BIT(4),
		TRASNFER_DST		= BIT(5),
	};
	enum ImageLayout
	{
		LAYOUT_UNDEFINED		= 0,
		COLOR_OPTIMAL			= BIT(1),
		DEPTH_OPTIMAL			= BIT(2),
		SHADER_READ_ONLY		= BIT(3),
	};
	struct TextureInfo
	{
		uint32_t width{ 1 }, height{ 1 };
		uint32_t mipMapLevel{ 1 };
		uint32_t samples{ 1 };
		ImageFormat imageFormat;
		ImageAspect imageAspect;
		ImageLayout imageLayout;
		uint32_t imageUsage;
	};

	class Texture
	{
	public:
		virtual ~Texture() = default;

		inline const TextureInfo& GetInfo() const { return m_textureInfo; }

		virtual void SetData(void* data, uint32_t size) = 0;
		virtual void Bind() const = 0;
		virtual void Dispose() = 0;

	protected:
		TextureInfo m_textureInfo;
		std::string m_path;
	};

	class Texture2D : public Texture
	{
	public:
		Texture2D(const TextureInfo& textureInfo, const std::string& path) { m_textureInfo = textureInfo; m_path = path; }
		static Ref<Texture2D> Create(const TextureInfo& textureInfo, const std::string& path = "");
	};
}