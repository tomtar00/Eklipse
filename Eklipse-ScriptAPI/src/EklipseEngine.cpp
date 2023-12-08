#include "EklipseEngine.h"
#include "ScriptAPI.h"

#include <Eklipse.h>

namespace EklipseEngine
{
    EK_API void Log(const char* message, ...)
    {		
        va_list args;
        ScriptAPI::Logger->info(message, args);
    }
    EK_API void LogWarn(const char* message, ...)
    {
        va_list args;
        ScriptAPI::Logger->warn(message, args);
    }
    EK_API void LogError(const char* message, ...)
    {
        va_list args;
        ScriptAPI::Logger->error(message, args);
    }
}