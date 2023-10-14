#pragma once
//
//#include "Framebuffer.h"
//#include "VertexArray.h"
//
//namespace Eklipse
//{
//	enum ViewportFlags
//	{
//		VIEWPORT_NONE				= 0, 
//		VIEWPORT_BLIT_FRAMEBUFFER	= BIT(1),
//		VIEWPORT_FULLSCREEN			= BIT(2)
//	};
//	struct ViewportCreateInfo
//	{
//		int flags;
//		FramebufferInfo framebufferInfo;
//	};
//
//	class Viewport
//	{
//	public:
//		static Ref<Viewport> Create(ViewportCreateInfo& info);
//
//		Viewport(ViewportCreateInfo& info);
//		virtual ~Viewport() = default;
//
//		virtual void BindFramebuffer()= 0;
//		virtual void UnbindFramebuffer() = 0;
//		virtual void Resize(uint32_t width, uint32_t height);
//		virtual void Bind();
//
//		inline Ref<VertexArray> GetVertexArray() const { return m_vertexArray;  }
//		inline ViewportCreateInfo& GetCreateInfo() { return m_createInfo; }
//		inline bool HasFlags(ViewportFlags flags) { return m_createInfo.flags & flags; }
//
//	protected:
//		ViewportCreateInfo m_createInfo;
//		Ref<VertexArray> m_vertexArray;
//	};
//}