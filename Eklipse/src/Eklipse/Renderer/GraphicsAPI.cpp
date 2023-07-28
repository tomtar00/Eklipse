#include "precompiled.h"
#include "GraphicsAPI.h"

namespace Eklipse
{
    GraphicsAPI::GraphicsAPI() : 
        m_apiType(ApiType::None) {}

    bool GraphicsAPI::IsInitialized()
    {
        return m_initialized;
    }
    ApiType GraphicsAPI::GetApiType()
    {
        return m_apiType;
    }

    void GraphicsAPI::SetApiType(ApiType apiType)
    {
        m_apiType = apiType;
    }
}
