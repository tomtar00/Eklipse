#include "EklipseEngine.h"
#include <Eklipse.h>

EK_API void EklipseEngine::Log(const char* message)
{
	EK_INFO(message);
}