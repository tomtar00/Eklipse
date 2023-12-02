#include "EklipseEngine.h"
#include <Eklipse.h>

void EklipseEngine::Log(const char* message)
{
	EK_INFO(message);
}