#include "precompiled.h"

//#include "Viewport.h"
//
//#include "Renderer.h"
//#include <Eklipse/Platform/OpenGL/GLViewport.h>
//#include <Eklipse/Platform/Vulkan/VKViewport.h>
//
//namespace Eklipse
//{
//	Ref<Viewport> Viewport::Create(ViewportCreateInfo& info)
//	{
//		auto apiType = Renderer::GetAPI();
//		switch (apiType)
//		{
//			case ApiType::Vulkan: return CreateRef<Vulkan::VKViewport>(info);
//			case ApiType::OpenGL: return CreateRef<OpenGL::GLViewport>(info);
//		}
//		EK_ASSERT(false, "API {0} not implemented for RenderTarget creation", int(apiType));
//		return nullptr;
//	}
//
//	Viewport::Viewport(ViewportCreateInfo& info) : m_createInfo(info)
//	{
//		if (info.flags & VIEWPORT_FULLSCREEN)
//		{
//			std::vector<float> vertices = {
//				 1.0f,  1.0f, 1.0f, 1.0f,  // top right
//				 1.0f, -1.0f, 1.0f, 0.0f,  // bottom right
//				-1.0f, -1.0f, 0.0f, 0.0f,  // bottom left
//				-1.0f,  1.0f, 0.0f, 1.0f,  // top left
//			};
//			std::vector<uint32_t> indices = {
//				0, 1, 3,
//				1, 2, 3
//			};
//
//			Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(vertices);
//			BufferLayout layout = {
//				{ "inPos",			ShaderDataType::Float2,		false },
//				{ "inTexCoords",	ShaderDataType::Float2,		false },
//			};
//			vertexBuffer->SetLayout(layout);
//
//			m_vertexArray = VertexArray::Create();
//			m_vertexArray->AddVertexBuffer(vertexBuffer);
//			m_vertexArray->SetIndexBuffer(IndexBuffer::Create(indices));
//		}
//	}
//	void Viewport::Resize(uint32_t width, uint32_t height)
//	{
//		g_viewportSize = { width, height };
//		g_aspectRatio = (float)width / (float)height;
//	}
//	void Viewport::Bind()
//	{
//		if (m_createInfo.flags & VIEWPORT_FULLSCREEN)
//		{
//			m_vertexArray->Bind();
//		}
//	}
//}