#include "precompiled.h"
#include "Window.h"
#include "Input.h"
#include <Eklipse/Platform/Windows/WindowsWindow.h>

namespace Eklipse
{
    void Window::GetFramebufferSize(int& width, int& height)
    {
        width = m_data.width;
        height = m_data.height;
    }
    Ref<Window> Window::Create(WindowData& data)
    {
#ifdef EK_PLATFORM_WINDOWS
        return CreateRef<WindowsWindow>(data);
#else
        #error Window for this plaform is not supported
#endif
    }
}
