#include "precompiled.h"
#include "EklipseEngine.h"
#include <EklipseScriptAPI.h>

namespace EklipseEngine
{
    template<typename ... Args> EK_API void _Log(const char* message, Args && ...args)
    {
        EklipseScriptAPI::Logger->info(message, std::forward<Args>(args)...);
    }
    template<typename ... Args> EK_API void _LogWarn(const char* message, Args && ...args)
    {
        EklipseScriptAPI::Logger->warn(message, std::forward<Args>(args)...);
    }
    template<typename ... Args> EK_API void _LogError(const char* message, Args && ...args)
    {
        EklipseScriptAPI::Logger->error(message, std::forward<Args>(args)...);
    }
}

EK_API void EklipseEngine::Log(const std::string& message, ...)
{
    //va_list args;
    //va_start(args, message.c_str());

    EklipseScriptAPI::Logger->info(message);

    //va_end(args);
}

EK_API void EklipseEngine::LogWarn(const std::string& message, ...)
{
    //va_list args;
    //va_start(args, message);

    EklipseScriptAPI::Logger->warn(message);

    //va_end(args);
}

EK_API void EklipseEngine::LogError(const std::string& message, ...)
{
    //va_list args;
    //va_start(args, message);

    EklipseScriptAPI::Logger->error(message);

    //va_end(args);
}