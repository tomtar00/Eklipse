#include "precompiled.h"
#include "GraphicsAPI.h"

namespace Eklipse
{
    GraphicsAPI::GraphicsAPI() : 
        m_initialized(false),
        m_scene(nullptr) {}

    Scene* GraphicsAPI::GetScene()
    {
        return m_scene;
    }

    bool GraphicsAPI::IsInitialized()
    {
        return m_initialized;
    }
}
