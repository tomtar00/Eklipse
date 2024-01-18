#include "precompiled.h"
#include "EklipseEngine.h"
#include <EklipseScriptAPI.h>

void EklipseEngine::Log(const std::string& message, ...)
{
    //va_list args;
    //va_start(args, message.c_str());

    EklipseScriptAPI::Logger->info(message);

    //va_end(args);
}

void EklipseEngine::LogWarn(const std::string& message, ...)
{
    //va_list args;
    //va_start(args, message);

    EklipseScriptAPI::Logger->warn(message);

    //va_end(args);
}

void EklipseEngine::LogError(const std::string& message, ...)
{
    //va_list args;
    //va_start(args, message);

    EklipseScriptAPI::Logger->error(message);

    //va_end(args);
}