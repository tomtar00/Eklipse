#pragma once

#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <sstream>
#include <functional>
#include <map>
#include <unordered_map>
#include <set>
#include <stdexcept>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <random>
#include <stacktrace>
#include <optional>
#include <thread>
#include <stack>
#include <regex>
#include <array>
#include <cstdarg>

#include "Internal.h"

namespace Eklipse
{
	EK_API void Log(const char* message, ...);
	EK_API void LogWarn(const char* message, ...);
	EK_API void LogError(const char* message, ...);
}