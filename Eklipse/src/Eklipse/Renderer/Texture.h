#pragma once

namespace Eklipse
{
	struct TextureData // TODO: remove
	{
		int width, height;
		void* pixels;
	};
	enum class ImageFormat
	{
		None	= 0,
		R8		= BIT(1),
		RGB8	= BIT(2),
		RGBA8	= BIT(3),
		RGBA32F = BIT(4)
	};
	struct TextureInfo
	{
		uint32_t width{ 1 }, height{ 1 };
		uint32_t mipMapLevel{ 1 };
		ImageFormat imageFormat;
	};

	class Texture
	{
	public:
		virtual ~Texture() = default;

		inline const TextureInfo& GetInfo() const { return m_textureInfo; }

		virtual void SetData(void* data, uint32_t size) = 0;
		virtual void Bind() const = 0;

	protected:
		TextureInfo m_textureInfo;
	};

	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> Create(const std::string& texturePath);
		static Ref<Texture2D> Create(const TextureInfo& textureInfo);
	};
}