#pragma once
//#include <Eklipse/Renderer/Viewport.h>
//#include "GLFramebuffer.h"
//#include "GLVertexArray.h"
//#include "GLBuffers.h"
//
//namespace Eklipse
//{
//	namespace OpenGL
//	{
//		class GLViewport : public Eklipse::Viewport
//		{
//		public:
//			GLViewport(ViewportCreateInfo& info);
//			~GLViewport() = default;
//
//			virtual void BindFramebuffer() override;
//			virtual void UnbindFramebuffer() override;
//			virtual void Resize(uint32_t width, uint32_t height) override;
//			virtual void Bind() override;
//			
//		private:
//			Ref<GLFramebuffer> m_framebuffer;
//			Ref<GLFramebuffer> m_blitFramebuffer;
//		};
//	}
//}