#include "precompiled.h"
#include "EklipseEngine.h"
#include "EklipseScriptAPI.h"

#include <Eklipse.h>
#include <cstdarg>

namespace EklipseEngine
{
    EK_API void Log(const char* message, ...)
    {		
        va_list args;
        va_start(args, message);
        EklipseScriptAPI::Logger->info(message, args);
        va_end(args);
    }
    EK_API void LogWarn(const char* message, ...)
    {
        va_list args;
        va_start(args, message);
        EklipseScriptAPI::Logger->warn(message, args);
        va_end(args);
    }
    EK_API void LogError(const char* message, ...)
    {
        va_list args;
        va_start(args, message);
        EklipseScriptAPI::Logger->error(message, args);
        va_end(args);
    }
}